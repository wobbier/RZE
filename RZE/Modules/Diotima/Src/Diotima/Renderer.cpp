#include <Diotima/Renderer.h>

#include <Brofiler/Brofiler.h>

#include <Utils/Conversions.h>
#include <Utils/MemoryUtils.h>
#include <Utils/DebugUtils/Debug.h>
#include <Utils/Math/Vector4D.h>
#include <Utils/Platform/FilePath.h>

// DX12 Branch Temp
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

			mRenderItems[index] = std::move(itemProtocol);
			mRenderItems[index].bIsValid = true;

			return index;
		}

		mRenderItems.emplace_back(std::move(itemProtocol));
		mRenderItems.back().bIsValid = true;

		return static_cast<Int32>(mRenderItems.size() - 1);
	}

	void Renderer::RemoveRenderItem(const U32 itemIdx)
	{
		AssertExpr(itemIdx < mRenderItems.size());

		mRenderItems[itemIdx].Invalidate();
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

		PrepareLights();
		PrepareDrawCalls();

		BuildCommandList();
	}

	void Renderer::Render()
	{
		mDevice->Present();
	}

	void Renderer::ShutDown()
	{
		mDevice->Shutdown();
	}

	void Renderer::DX12Initialize()
	{
		mDevice = std::make_unique<DX12GFXDevice>();
		mDevice->SetWindow(mWindowHandle);
		mDevice->SetMSAASampleCount(mMSAASampleCount);
		mDevice->Initialize();

		mMVPConstantBuffer = mDevice->CreateConstantBuffer(sizeof(Matrix4x4) * 3, 1024);
		mLightConstantBuffer = mDevice->CreateConstantBuffer(sizeof(LightItemProtocol), 64);
		mMaterialBuffer = mDevice->CreateConstantBuffer(sizeof(RenderItemMaterialDesc), 1024);
		mPerFramePixelShaderConstants = mDevice->CreateConstantBuffer(sizeof(U32) * 2, 1024);
	}

	void Renderer::PrepareLights()
	{
		std::vector<LightItemProtocol> directionalLights;
		std::vector<LightItemProtocol> pointLights;

		for (LightItemProtocol light : mLightingList)
		{
			if (light.LightType == ELightType::Directional)
			{
				directionalLights.push_back(light);
			}
			else
			{
				pointLights.push_back(light);
			}
		}

		DX12GFXConstantBuffer* const lightConstantBuffer = mDevice->GetConstantBuffer(mLightConstantBuffer);
		lightConstantBuffer->Reset();

		for (LightItemProtocol& light : pointLights)
		{
			lightConstantBuffer->AllocateMember(&light);
		}
		//lightConstantBuffer->SetData(directionalLights.data(), static_cast<U32>(sizeof(LightItemProtocol) * directionalLights.size()), 1);

		DX12GFXConstantBuffer* const perFramePixelShaderConstants = mDevice->GetConstantBuffer(mPerFramePixelShaderConstants);
		perFramePixelShaderConstants->Reset();
		U32 lightCounts[2] = { static_cast<U32>(pointLights.size()), static_cast<U32>(directionalLights.size()) };
		perFramePixelShaderConstants->AllocateMember(lightCounts);
	}

	void Renderer::PrepareDrawCalls()
	{
		BROFILER_CATEGORY("Renderer::PrepareDrawCalls", Profiler::Color::Red);
		if (mPerFrameDrawCalls.capacity() < mRenderItems.size())
		{
			mPerFrameDrawCalls.reserve(mRenderItems.size());
		}

		Matrix4x4 camViewProjMat = camera.ProjectionMat * camera.ViewMat;
		void* pMatrixConstantBufferData = malloc(sizeof(Matrix4x4) * 3);

		DX12GFXConstantBuffer* const materialBuffer = mDevice->GetConstantBuffer(mMaterialBuffer);
		DX12GFXConstantBuffer* const matrixBuffer = mDevice->GetConstantBuffer(mMVPConstantBuffer);

		matrixBuffer->Reset();
		materialBuffer->Reset();

		mPerFrameDrawCalls.clear();
		for (RenderItemProtocol& renderItem : mRenderItems)
		{
			// #TODO(Josh::This needs to be removed -- an opaque handle should be leased out that will
			//             get fixed up when we remove render items)
			if (!renderItem.bIsValid)
			{
				continue;
			}

			CBAllocationData matrixSlot;
			// #TODO(Josh::Just copy pasta from before for now to get RenderItemDrawCall driving the Renderer::Update loop
				//             Should find a better place for it.)
			{
				const float* modelViewPtr = renderItem.ModelMatrix.GetValuePtr();
				const float* modelViewInvPtr = renderItem.ModelMatrix.Inverse().GetValuePtr();
				const float* camViewProjPtr = camViewProjMat.GetValuePtr();

				memcpy(pMatrixConstantBufferData, modelViewPtr, sizeof(Matrix4x4));
				memcpy((U8*)pMatrixConstantBufferData + sizeof(Matrix4x4), modelViewInvPtr, sizeof(Matrix4x4));
				memcpy((U8*)pMatrixConstantBufferData + sizeof(Matrix4x4) * 2, camViewProjPtr, sizeof(Matrix4x4));

				matrixSlot = matrixBuffer->AllocateMember(pMatrixConstantBufferData);
			}

			for (RenderItemMeshData& meshData : renderItem.MeshData)
			{
				RenderItemDrawCall drawCall;
				drawCall.VertexBuffer = meshData.VertexBuffer;
				drawCall.IndexBuffer = meshData.IndexBuffer;
				// #TODO(Josh::Eventually the material system will handle this itself and hold a buffer of pre-allocated materials
				//             and we will somehow "lease" it for the draw call)
				drawCall.MaterialSlot = materialBuffer->AllocateMember(&meshData.Material);
				drawCall.TextureSlot = meshData.TextureDescs[0].TextureBuffer; // Currently [0] is diffuse, but this should be iterated to be more robust.
				drawCall.MatrixSlot = matrixSlot;

				mPerFrameDrawCalls.push_back(std::move(drawCall));
			}
		}

		delete pMatrixConstantBufferData;
	}

	void Renderer::BuildCommandList()
	{
		mDevice->ResetCommandAllocator();
		mDevice->ResetCommandList();

		mDevice->BeginFrame();
		{
			ID3D12GraphicsCommandList* commandList = mDevice->GetCommandList();
			DX12GFXConstantBuffer* const lightConstantBuffer = mDevice->GetConstantBuffer(mLightConstantBuffer);
			DX12GFXConstantBuffer* const perFramePixelShaderConstants = mDevice->GetConstantBuffer(mPerFramePixelShaderConstants);

			ID3D12DescriptorHeap* ppDescHeaps[] = { mDevice->GetTextureHeap() };
			commandList->SetDescriptorHeaps(_countof(ppDescHeaps), ppDescHeaps);

			commandList->SetGraphicsRootConstantBufferView(2, lightConstantBuffer->GetResource()->GetGPUVirtualAddress());
			commandList->SetGraphicsRootConstantBufferView(5, perFramePixelShaderConstants->GetResource()->GetGPUVirtualAddress());

			commandList->SetGraphicsRoot32BitConstants(1, 3, &camera.Position.GetInternalVec(), 0);

			for (RenderItemDrawCall& drawCall : mPerFrameDrawCalls)
			{
				commandList->SetGraphicsRootConstantBufferView(0, drawCall.MatrixSlot.GPUBaseAddr);
				commandList->SetGraphicsRootConstantBufferView(4, drawCall.MaterialSlot.GPUBaseAddr);

				// #NOTE(Josh::Everything should have a default guaranteed diffuse map. For now it also marks the start of the descriptor table)
				DX12GFXTextureBuffer2D* const diffuseBuffer = mDevice->GetTextureBuffer2D(drawCall.TextureSlot);
				commandList->SetGraphicsRootDescriptorTable(3, diffuseBuffer->GetDescriptorHandleGPU());

				DX12GFXVertexBuffer* const vertexBuffer = mDevice->GetVertexBuffer(drawCall.VertexBuffer);
				DX12GFXIndexBuffer* const indexBuffer = mDevice->GetIndexBuffer(drawCall.IndexBuffer);

				commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				commandList->IASetVertexBuffers(0, 1, vertexBuffer->GetBufferView());
				commandList->IASetIndexBuffer(indexBuffer->GetBufferView());
				commandList->DrawIndexedInstanced(indexBuffer->GetNumElements(), 1, 0, 0, 0);
			}
		}
		mDevice->EndFrame();
	}

	void Renderer::EnableVsync(bool bEnabled)
	{
		
	}

	void Renderer::SetMSAASampleCount(U32 sampleCount)
	{
		mMSAASampleCount = sampleCount;
	}

	void Renderer::ResizeCanvas(const Vector2D& newSize)
	{
		mCanvasSize = newSize;

		LOG_CONSOLE_ARGS("New Canvas Size: %f x %f", mCanvasSize.X(), mCanvasSize.Y());
	}

	U32 Renderer::CreateVertexBuffer(void* data, U32 numElements)
	{
		return mDevice->CreateVertexBuffer(data, numElements);
	}

	U32 Renderer::CreateIndexBuffer(void* data, U32 numElements)
	{
		return mDevice->CreateIndexBuffer(data, numElements);
	}

	U32 Renderer::CreateTextureBuffer2D(void* data, U32 width, U32 height)
	{
		return mDevice->CreateTextureBuffer2D(data, width, height);
	}

	void Renderer::RenderItemProtocol::Invalidate()
	{
		bIsValid = false;
	}

}
