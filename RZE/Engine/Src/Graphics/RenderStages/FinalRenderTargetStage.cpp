#include <StdAfx.h>
#include <Graphics/RenderStages/FinalRenderTargetStage.h>

#include <Graphics/RenderEngine.h>

#include <Rendering/Renderer.h>
#include <Rendering/Graphics/RenderTarget.h>

void FinalRenderTargetStage::Initialize()
{
	{
		Rendering::ShaderInputLayout inputLayout = {};

		m_vertexShaderResource = RZE().GetResourceHandler().LoadResource<VertexShader>(Filepath("Assets/Shaders/Vertex_RenderTargetQuad.hlsl"), "Vertex_RenderTargetQuad", inputLayout);
		AssertExpr(m_vertexShaderResource.IsValid());
		m_vertexShader = RZE().GetResourceHandler().GetResource<VertexShader>(m_vertexShaderResource);
	}

	{
		m_pixelShaderResource = RZE().GetResourceHandler().LoadResource<PixelShader>(Filepath("Assets/Shaders/Pixel_RenderToTarget.hlsl"), "Pixel_RenderToTarget");
		AssertExpr(m_pixelShaderResource.IsValid());
		m_pixelShader = RZE().GetResourceHandler().GetResource<PixelShader>(m_pixelShaderResource);
	}
}

void FinalRenderTargetStage::Update(const RenderStageData& renderData)
{
}

void FinalRenderTargetStage::Render(const RenderStageData& renderData)
{
	Rendering::Renderer::Begin("FinalRenderTargetStage");

	RenderEngine& engine = RZE().GetRenderEngine();
	const Rendering::RenderTargetTexture& target = engine.GetRenderTarget();

	Rendering::Renderer::SetRenderTarget(nullptr);
	Rendering::Renderer::SetRenderTargetBackBuffer();

	Rendering::Renderer::SetPrimitiveTopology(Rendering::EPrimitiveTopology::TriangleStrip);

	Rendering::Renderer::SetVertexShader(m_vertexShader->GetPlatformObject());
	Rendering::Renderer::SetPixelShader(m_pixelShader->GetPlatformObject());
	
	Rendering::Renderer::SetTextureResource(target.GetTargetPlatformObject(), 0);
	
	Rendering::Renderer::DrawFullScreenQuad();

	Rendering::Renderer::UnsetTextureResource(0);

	Rendering::Renderer::End();
}
