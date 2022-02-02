#pragma once

#include <Rendering/Driver/GFXBuffer.h>

struct ID3D11Buffer;

namespace Rendering
{
	class DX11Device;

	class DX11GFXIndexBuffer : public IGFXIndexBuffer
	{
	public:
		DX11GFXIndexBuffer() = default;
		~DX11GFXIndexBuffer() = default;

	// IGFXIndexBuffer interface
	public:
		virtual void Allocate(void* data, size_t size, U32 count) override;
		virtual void Release() override;

	public:
		void SetDevice(DX11Device* device);
		ID3D11Buffer& GetHardwareBuffer();

		U32 GetIndexCount();

	private:
		U32 mIndexCount;

	private:
		DX11Device* mDevice;
		ID3D11Buffer* mBuf;
	};

}