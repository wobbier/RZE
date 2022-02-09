#pragma once

#include <Rendering/Driver/ShaderTypes.h>

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct D3D11_INPUT_ELEMENT_DESC;

namespace Rendering
{
	class DX11Device;

	class DX11VertexShader : public IVertexShader
	{
	public:
		DX11VertexShader();
		~DX11VertexShader() override;

	public:
		void Create(const FilePath& filePath, const ShaderInputLayout& inputLayout) override;
		void Release() override;
		void SetActive() override;
		void SetInputLayout() override;
		
	public:
		void SetDevice(DX11Device* graphicsDevice);
		

	private:
		ID3D11VertexShader* m_hwShader = nullptr;
		ID3D11InputLayout* m_hwInputLayout = nullptr;
		DX11Device* m_graphicsDevice;
	};

	class DX11PixelShader : public IPixelShader
	{
	public:
		DX11PixelShader();
		~DX11PixelShader() override;

	public:
		void Create(const FilePath& filePath) override;
		void Release() override;
		void SetActive() override;

	public:
		void SetDevice(DX11Device* graphicsDevice);

	private:
		ID3D11PixelShader* m_hwShader = nullptr;
		DX11Device* m_graphicsDevice;
	};
}