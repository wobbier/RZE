#include <Rendering/Renderer.h>

#include <Optick/optick.h>

#include <Utils/Conversions.h>
#include <Utils/DebugUtils/Debug.h>
#include <Utils/Math/Vector4D.h>
#include <Utils/Platform/FilePath.h>

// DX11
#include <Rendering/Driver/DX11/DX11Device.h>
#include <Rendering/Driver/DX11/DX11.h>
#include <Rendering/Driver/DX11/DX11VertexBuffer.h>

#include <array>

namespace
{
	D3D11_INPUT_ELEMENT_DESC k_vertLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numLayoutElements = ARRAYSIZE(k_vertLayout);
	
	// Only used to test preliminary changes to how the renderer functions without
	// having to write too much code that won't last
	struct DrawStateData_Prototype
	{
		ID3D10Blob* mVSBlob = nullptr;
		ID3D11VertexShader* mVertexShader = nullptr;
		ID3D11InputLayout* mVertexLayout = nullptr;
		Rendering::IGFXConstantBuffer* mCameraDataBuffer = nullptr;
		
		void Initialize(Rendering::DX11Device* hwDevice)
		{
			//mCameraDataBuffer = hwDevice->CreateConstantBuffer(MemoryUtils::AlignSize(sizeof(CameraGPUData), 16), 1);

			// Shaders
			{
				FilePath vertexShaderPath("Assets/Shaders/Vertex_NewRenderer.hlsl");

				HRESULT hr;
				ID3D10Blob* error;
				
				auto errorCB = [](ID3D10Blob* error)
				{
					if (error != nullptr)
					{
						char const* message =
							static_cast<char const*>(error->GetBufferPointer());

						// Write the warning to the output window when the program is
						// executing through the Microsoft Visual Studio IDE.
						size_t const length = strlen(message);
						std::wstring output = L"";
						for (size_t i = 0; i < length; ++i)
						{
							output += static_cast<wchar_t>(message[i]);
						}
						output += L'\n';
						OutputDebugString(output.c_str());
					}
				};

				hr = D3DCompileFromFile(Conversions::StringToWString(vertexShaderPath.GetAbsolutePath()).c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0", 0, 0, &mVSBlob, &error);
				errorCB(error);

				hr = hwDevice->GetHardwareDevice().CreateVertexShader(mVSBlob->GetBufferPointer(), mVSBlob->GetBufferSize(), nullptr, &mVertexShader);
				hr = hwDevice->GetHardwareDevice().CreateInputLayout(k_vertLayout, numLayoutElements, mVSBlob->GetBufferPointer(), mVSBlob->GetBufferSize(), &mVertexLayout);
			}
		}
	};
}

namespace Rendering
{
	std::unique_ptr<DX11Device> Renderer::m_device;

	Renderer::Renderer()
	{
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::Initialize(void* windowHandle)
	{
		m_device = std::make_unique<DX11Device>();
		m_device->SetWindow(windowHandle);
		m_device->Initialize();

#ifdef IMGUI_ENABLED
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(windowHandle);
		ImGui_ImplDX11_Init(&m_device->GetHardwareDevice(), &m_device->GetDeviceContext());
#endif
	}
	
	void Renderer::Shutdown()
	{
#ifdef IMGUI_ENABLED
		ImGui_ImplDX11_Shutdown();
#endif
		m_device->Shutdown();
	}

	void Renderer::BeginFrame()
	{
	}

	void Renderer::EndFrame()
	{
		m_device->Present();
	}

	void Renderer::Begin()
	{
	}

	void Renderer::End()
	{
	}

	VertexBufferHandle Renderer::CreateVertexBuffer(void* data, size_t dataTypeSize, size_t count)
	{
		std::shared_ptr<DX11VertexBuffer> vertexBuffer = std::make_shared<DX11VertexBuffer>();
		vertexBuffer->SetDevice(m_device.get());
		vertexBuffer->Allocate(data, dataTypeSize, count);

		return VertexBufferHandle(vertexBuffer);
	}

	void Renderer::SetClearColour(const Vector4D& colour)
	{
		ID3D11DeviceContext& deviceContext = m_device->GetDeviceContext();
		// @TODO Move render target off device and set it on render state
		FLOAT rgba[4] = { colour.X(), colour.Y(), colour.Z(), colour.W() };
		deviceContext.ClearRenderTargetView(m_device->mRenderTargetView, rgba);
	}
}
