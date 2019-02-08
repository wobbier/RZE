#pragma once

#include <memory>

#include <wrl.h>
#include <Diotima/Driver/DX12/d3dx12.h>
#include <dxgi.h>
#include <dxgi1_4.h>

#include <Diotima/Driver/GFXDevice.h> 

#include <Utils/PrimitiveDefs.h>

using namespace Microsoft::WRL;

// struct ID3D12Device;
// struct ID3D12CommandQueue;
// struct ID3D12CommandAllocator;
// struct ID3D12DescriptorHeap;
// struct ID3D12Resource;
// struct ID3D12RootSignature;
// struct ID3D12PipelineState;
// struct ID3D12GraphicsCommandList;
// struct ID3D12Fence;
// 
// struct IDXGIFactory4;
// struct IDXGISwapChain3;
// 
// struct D3D12_VERTEX_BUFFER_VIEW;
// struct D3D12_VIEWPORT;
// struct D3D12_RECT;

namespace Diotima
{
	// #TODO(Josh::Eventually move these into a more configurable/stateful place)
	constexpr int kBufferCount = 2;
	constexpr int kBufferWidth = 1920;
	constexpr int kBufferHeight = 1080;

	class DX12GFXDevice : public IGFXDevice
	{
		friend class DX12GFXDriverInterface;

	public:
		DX12GFXDevice();
		virtual ~DX12GFXDevice();

	public:
		virtual void Initialize() final override;
		// #TODO(Josh::Really really don't like this, fix later)
		virtual void SetWindow(void* windowHandle) final override;

		void OnRender();

	private:
		void InitializeAssets();
		void PopulateCommandList();
		void WaitForPreviousFrame();


	private:
		ComPtr<ID3D12Device> mDevice;
		ComPtr<ID3D12CommandQueue> mCommandQueue;
		ComPtr<ID3D12CommandAllocator> mCommandAllocator;
		ComPtr<ID3D12DescriptorHeap> mRTVDescriptorHeap;
		ComPtr<ID3D12Resource> mRenderTargets[kBufferCount];
		ComPtr<ID3D12RootSignature> mRootSignature;
		ComPtr<ID3D12PipelineState> mPipelineState;
		ComPtr<ID3D12GraphicsCommandList> mCommandList;

		ComPtr<IDXGIFactory4> mFactory;
		ComPtr<IDXGISwapChain3> mSwapChain;

		ComPtr<ID3D12Resource> mVertexBuffer;

		D3D12_VIEWPORT* mViewport;
		D3D12_RECT mScissorRect;
		D3D12_VERTEX_BUFFER_VIEW* mVertexBufferView;

		int mCurrentFrame;
		void* mWindowHandle;

		U32 mRTVDescriptorSize;

		// Fence stuff
		U32 mFrameIndex;
		HANDLE mFenceEvent;
		ComPtr<ID3D12Fence> mFence;
		U64 mFenceValue;
	};
}