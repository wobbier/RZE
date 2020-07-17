#include <Diotima/Renderer.h>

#include <Optick/optick.h>

#include <Diotima/Graphics/GFXPassGraph.h>

#include <Perseus/JobSystem/JobScheduler.h>

#include <Utils/Conversions.h>
#include <Utils/MemoryUtils.h>
#include <Utils/DebugUtils/Debug.h>
#include <Utils/Math/Vector4D.h>
#include <Utils/Platform/FilePath.h>

// DX11
#include <Diotima/Driver/DX11/DX11GFXDevice.h>
#include <Diotima/Driver/DX11/DX11.h>
// #TODO(What makes this needed here? unique_ptr complains re: destructor visibility
//       is it necessary otherwise?)
#include <Diotima/Driver/DX11/DX11GFXVertexBuffer.h>
#include <Diotima/Driver/DX11/DX11GFXIndexBuffer.h>
#include <Diotima/Driver/DX11/DX11GFXConstantBuffer.h>
#include <Diotima/Driver/DX11/DX11GFXTextureBuffer2D.h>
#include <Diotima/Graphics/RenderTarget.h>


#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx11.h>
#include <ImGui/imgui_impl_win32.h>

#include <array>

namespace Diotima
{

	Renderer::Renderer()
		: mRenderTarget(nullptr)
	{
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::Initialize()
	{
		mCanvasSize.SetXY(1600, 900);

		mDevice = std::make_unique<DX11GFXDevice>();
		mDevice->SetWindow(mWindowHandle);
		mDevice->Initialize();

		ImGui::CreateContext();
		ImGui_ImplWin32_Init(mWindowHandle);
		ImGui_ImplDX11_Init(&mDevice->GetHardwareDevice(), &mDevice->GetDeviceContext());
	}

	void Renderer::Update()
	{
		OPTICK_EVENT();

		ProcessCommands();
	}

	void Renderer::Render()
	{
		if (mRenderTarget != nullptr)
		{
			ID3D11RenderTargetView* nullViews[] = { nullptr };
			mDevice->GetDeviceContext().OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);

			mDevice->GetDeviceContext().OMSetRenderTargets(1, &mDevice->mRenderTargetView, mDevice->mDepthStencilView);

			FLOAT rgba[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			mDevice->GetDeviceContext().ClearRenderTargetView(mDevice->mRenderTargetView, rgba);
		}

		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		mDevice->Present();
	}

	void Renderer::ShutDown()
	{
		ImGui_ImplDX11_Shutdown();
		mDevice->Shutdown();
	}

	void Renderer::EnableVsync(bool bEnabled)
	{
		mDevice->SetSyncInterval(static_cast<U32>(bEnabled));
	}
	
	const Vector2D& Renderer::GetCanvasSize()
	{
		return mCanvasSize;
	}

	// #TODO
	// Take with new renderer
	void Renderer::ResizeCanvas(const Vector2D& newSize)
	{
		mCanvasSize = newSize;

		int width = static_cast<int>(newSize.X());
		int height = static_cast<int>(newSize.Y());

		ImGui::GetIO().DisplaySize.x = static_cast<float>(width);
		ImGui::GetIO().DisplaySize.y = static_cast<float>(height);

		mDevice->HandleWindowResize(newSize);

		LOG_CONSOLE_ARGS("New Canvas Size: %f x %f", mCanvasSize.X(), mCanvasSize.Y());
	}

	// #TODO
	// Take with new renderer - only used in hacky RenderTarget code in EditorApp
	void Renderer::SetViewportSize(const Vector2D& newSize)
	{
		mViewportDimensions = newSize;
	}

	const Vector2D& Renderer::GetViewportSize()
	{
		return mViewportDimensions;
	}

	U32 Renderer::CreateVertexBuffer(void* data, size_t size, U32 count)
	{
		return mDevice->CreateVertexBuffer(data, size, count);
	}

	U32 Renderer::CreateIndexBuffer(void* data, size_t size, U32 count)
	{
		return mDevice->CreateIndexBuffer(data, size, count);
	}

	U32 Renderer::CreateTextureBuffer2D(void* data, U32 width, U32 height)
	{
		// This is temp here, should be passed in or something.
		GFXTextureBufferParams params = { 0 };
		params.bIsRenderTarget = true;
		params.bIsShaderResource = true;
		params.Height = height;
		params.Width = width;
		params.MipLevels = 0;
		params.MostDetailedMip = 0;
		params.SampleCount = 1;
		params.SampleQuality = 0;
		return mDevice->CreateTextureBuffer2D(data, params);
	}


	Diotima::RenderObjectHandle Renderer::CreateRenderObject()
	{
		RenderObjectHandle objectHandle;

		if (!mFreeRenderObjectIndices.empty())
		{
			U32 freeIndex = mFreeRenderObjectIndices.front();
			mFreeRenderObjectIndices.pop();
			objectHandle.Value = freeIndex;

			RenderObject& renderObject = mRenderObjects[freeIndex];
			renderObject = {};

			return objectHandle;
		}

		objectHandle.Value = mRenderObjects.size();
		mRenderObjects.emplace_back();

		return objectHandle;
	}

	void Renderer::ProcessCommands()
	{
		OPTICK_EVENT();
		
	}

}
