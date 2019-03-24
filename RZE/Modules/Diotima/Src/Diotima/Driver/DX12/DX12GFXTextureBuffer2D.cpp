#include <Diotima/Driver/DX12/DX12GFXTextureBuffer2D.h>

#include <Diotima/Driver/DX12/DX12GFXDevice.h>

#include <Utils/DebugUtils/Debug.h>

static U32 increment = 0;

namespace Diotima
{
	void DX12GFXTextureBuffer2D::Allocate(void* data, U32 width, U32 height)
	{
		mDevice->ResetCommandList();

		mResourceDesc = {};
		mResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		mResourceDesc.Alignment = 0;
		mResourceDesc.Width = width;
		mResourceDesc.Height = height;
		mResourceDesc.DepthOrArraySize = 1;
		mResourceDesc.MipLevels = 2;
		mResourceDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		mResourceDesc.SampleDesc.Count = 1;
		mResourceDesc.SampleDesc.Quality = 0;
		mResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		mResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		mDevice->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), 
			D3D12_HEAP_FLAG_NONE, 
			&mResourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST, 
			nullptr, 
			IID_PPV_ARGS(&mGPUBuffer));

		U64 uploadBufferRequiredSize = 0;
		mDevice->GetDevice()->GetCopyableFootprints(&mResourceDesc, 0, 1, 0, nullptr, nullptr, nullptr, &uploadBufferRequiredSize);

		mDevice->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), 
			D3D12_HEAP_FLAG_NONE, 
			&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferRequiredSize), 
			D3D12_RESOURCE_STATE_GENERIC_READ, 
			nullptr, 
			IID_PPV_ARGS(&mUploadBuffer));

		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = data;
		textureData.RowPitch = (32 * width) / 8;
		textureData.SlicePitch = textureData.RowPitch * height;

		ID3D12GraphicsCommandList* const commandList = mDevice->GetCommandList();
		UpdateSubresources(commandList, mGPUBuffer.Get(), mUploadBuffer.Get(), 0, 0, 1, &textureData);

		commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mGPUBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		commandList->Close();

		mDevice->ExecuteCommandList(commandList);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = mResourceDesc.Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		mSRVCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mDevice->GetTextureHeap()->GetCPUDescriptorHandleForHeapStart());
		mSRVCPUHandle.Offset(increment, mDevice->GetCBVSRVUAVDescriptorSize());

		mSRVGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mDevice->GetTextureHeap()->GetGPUDescriptorHandleForHeapStart());
		mSRVGPUHandle.Offset(increment, mDevice->GetCBVSRVUAVDescriptorSize());

		mDevice->GetDevice()->CreateShaderResourceView(mGPUBuffer.Get(), &srvDesc, mSRVCPUHandle);

		++increment;
	}

	void DX12GFXTextureBuffer2D::SetDevice(DX12GFXDevice* device)
	{
		AssertNotNull(device);
		mDevice = device;
	}

	ID3D12DescriptorHeap* DX12GFXTextureBuffer2D::GetDescriptorHeap()
	{
		return mDescriptorHeap.Get();
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE DX12GFXTextureBuffer2D::GetDescriptorHandleCPU()
	{
		return mSRVCPUHandle;
	}

	CD3DX12_GPU_DESCRIPTOR_HANDLE DX12GFXTextureBuffer2D::GetDescriptorHandleGPU()
	{
		return mSRVGPUHandle;
	}

	const D3D12_RESOURCE_DESC& DX12GFXTextureBuffer2D::GetResourceDesc()
	{
		return mResourceDesc;
	}

	ID3D12Resource* DX12GFXTextureBuffer2D::GetResource()
	{
		return mGPUBuffer.Get();
	}

}