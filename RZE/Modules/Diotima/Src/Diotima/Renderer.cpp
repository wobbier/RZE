#include <Diotima/Renderer.h>

#include <Brofiler/Brofiler.h>

#include <Diotima/Graphics/GFXMaterial.h>
#include <Diotima/Graphics/GFXMesh.h>
#include <Diotima/Graphics/GFXTexture2D.h>
#include <Diotima/Shaders/ShaderPipeline.h>

#include <Utils/Conversions.h>
#include <Utils/DebugUtils/Debug.h>
#include <Utils/Math/Vector4D.h>
#include <Utils/Platform/FilePath.h>

// DX12 Branch Temp
#include <Diotima/Driver/DX12/DX12GFXDriverInterface.h>
#include <Diotima/Driver/DX12/DX12GFXDevice.h>
#include <Diotima/Driver/DX12/DX12GFXConstantBuffer.h>
#include <Diotima/Driver/DX12/DX12GFXIndexBuffer.h>
#include <Diotima/Driver/DX12/DX12GFXVertexBuffer.h>
#include <Diotima/Driver/DX12/DX12GFXTextureBuffer2D.h>

namespace Diotima
{
	Renderer::Renderer()
	{
	}

	Renderer::~Renderer()
	{
	}

	Int32 Renderer::AddRenderItem(const RenderItemProtocol& itemProtocol)
	{
		if (!mFreeRenderListIndices.empty())
		{
			Int32 index = mFreeRenderListIndices.front();
			mFreeRenderListIndices.pop();

			mRenderList[index] = std::move(itemProtocol);
			mRenderList[index].bIsValid = true;

			return index;
		}

		mRenderList.emplace_back(std::move(itemProtocol));
		mRenderList.back().bIsValid = true;

		return static_cast<Int32>(mRenderList.size() - 1);
	}

	void Renderer::RemoveRenderItem(const U32 itemIdx)
	{
		AssertExpr(itemIdx < mRenderList.size());

		mRenderList[itemIdx].Invalidate();
		mFreeRenderListIndices.push(itemIdx);
	}

	Int32 Renderer::AddLightItem(const LightItemProtocol& itemProtocol)
	{
		mLightingList.emplace_back(std::move(itemProtocol));
		return static_cast<Int32>(mLightingList.size() - 1);
	}

	void Renderer::Initialize()
	{
		DX12Initialize();
	}

	void Renderer::Update()
	{
		BROFILER_CATEGORY("Renderer::Update", Profiler::Color::Red);

		DX12GFXDevice* const device = mDriverInterface->mDevice.get();
		device->ResetCommandAllocator();
		device->ResetCommandList();

		device->BeginFrame();
		{
			ID3D12GraphicsCommandList* commandList = device->GetCommandList();
			DX12GFXConstantBuffer* const MVPConstantBuffer = mDriverInterface->mDevice->GetConstantBuffer(mMVPConstantBuffer);
			DX12GFXConstantBuffer* const lightConstantBuffer = mDriverInterface->mDevice->GetConstantBuffer(mLightConstantBuffer);

			lightConstantBuffer->SetData(mLightingList.data(), sizeof(LightItemProtocol), 0);
			commandList->SetGraphicsRootConstantBufferView(2, lightConstantBuffer->GetResource()->GetGPUVirtualAddress());

			commandList->SetGraphicsRoot32BitConstants(1, 3, &camera.Position.GetInternalVec(), 0);

			U32 objectIndex = 0;
			for (RenderItemProtocol& itemProtocol : mRenderList)
			{
				// #TODO(Josh::This should go away when a better architecture is implemented. For now we assume
				//             everything is indexed and every mesh is 1:1 with their vert/index buffer count.)
				AssertEqual(itemProtocol.VertexBuffers.size(), itemProtocol.IndexBuffers.size());

				Matrix4x4 MVP = camera.ProjectionMat * camera.ViewMat * itemProtocol.ModelMatrix;
				MVPConstantBuffer->SetData(const_cast<float*>(MVP.GetValuePtr()), sizeof(float), objectIndex);

				commandList->SetGraphicsRootConstantBufferView(0, MVPConstantBuffer->GetResource()->GetGPUVirtualAddress() + (((sizeof(float) * 16) + 255) & ~255) * objectIndex);
				
				for (size_t index = 0; index < itemProtocol.VertexBuffers.size(); ++index)
				{
					U32 vertexBufferIndex = itemProtocol.VertexBuffers[index];
					U32 indexBufferIndex = itemProtocol.IndexBuffers[index];

					DX12GFXVertexBuffer* const vertexBuffer = device->GetVertexBuffer(vertexBufferIndex);
					DX12GFXIndexBuffer* const indexBuffer = device->GetIndexBuffer(indexBufferIndex);

					// #TODO(Josh::Just diffuse for now to test)
					DX12GFXTextureBuffer2D* const textureBuffer = device->GetTextureBuffer2D(itemProtocol.TextureBuffers[index]);
					ID3D12DescriptorHeap* ppDescHeaps[] = { textureBuffer->GetDescriptorHeap() };
					commandList->SetDescriptorHeaps(_countof(ppDescHeaps), ppDescHeaps);

					commandList->SetGraphicsRootDescriptorTable(3, textureBuffer->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());

					commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
					commandList->IASetVertexBuffers(0, 1, vertexBuffer->GetBufferView());
					commandList->IASetIndexBuffer(indexBuffer->GetBufferView());
					commandList->DrawIndexedInstanced(indexBuffer->GetNumElements(), 1, 0, 0, 0);
				}

				++objectIndex;
			}
		}
		device->EndFrame();
	}

	void Renderer::Render()
	{
		mDriverInterface->Present();
	}

	void Renderer::ShutDown()
	{
		mDriverInterface->Shutdown();
	}

	void Renderer::DX12Initialize()
	{
		mDriverInterface = std::make_unique<DX12GFXDriverInterface>();
		mDriverInterface->SetWindow(mWindowHandle);
		mDriverInterface->Initialize();

		mMVPConstantBuffer = mDriverInterface->CreateConstantBuffer(nullptr, sizeof(Matrix4x4));
		mLightConstantBuffer = mDriverInterface->CreateConstantBuffer(nullptr, 1);
	}

	void Renderer::EnableVsync(bool bEnabled)
	{
		
	}

	void Renderer::ResizeCanvas(const Vector2D& newSize)
	{
		mCanvasSize = newSize;

		LOG_CONSOLE_ARGS("New Canvas Size: %f x %f", mCanvasSize.X(), mCanvasSize.Y());
	}


	U32 Renderer::CreateVertexBuffer(void* data, U32 numElements)
	{
		return mDriverInterface->CreateVertexBuffer(data, numElements);
	}


	U32 Renderer::CreateIndexBuffer(void* data, U32 numElements)
	{
		return mDriverInterface->CreateIndexBuffer(data, numElements);
	}


	U32 Renderer::CreateTextureBuffer2D(void* data, U32 width, U32 height)
	{
		return mDriverInterface->CreateTextureBuffer2D(data, width, height);
	}

	Renderer::RenderItemProtocol::RenderItemProtocol()
	{
	}

	void Renderer::RenderItemProtocol::Invalidate()
	{
		bIsValid = false;
	}

}
