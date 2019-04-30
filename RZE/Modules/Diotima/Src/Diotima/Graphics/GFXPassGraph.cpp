#include <Diotima/Graphics/GFXPassGraph.h>

#include <Diotima/Renderer.h>
#include <Diotima/Graphics/RenderPasses/ForwardPass.h>
#include <Diotima/Graphics/RenderPasses/DepthPass.h>

#include <Utils/DebugUtils/Debug.h>

#include <Optick/optick.h>

namespace Diotima
{

	GFXPassGraph::GFXPassGraph()
	{
	}

	GFXPassGraph::~GFXPassGraph()
	{
	}

	void GFXPassGraph::Build(Renderer* renderer)
	{
		OPTICK_EVENT();

		std::unique_ptr<DepthPass> depthPass = std::make_unique<DepthPass>();
		depthPass->SetRenderer(renderer);
		depthPass->Initialize();

		mRenderPasses.push_back(std::move(depthPass));

		std::unique_ptr<ForwardPass> forwardPass = std::make_unique<ForwardPass>();
		forwardPass->SetRenderer(renderer);
		forwardPass->Initialize();

		mRenderPasses.push_back(std::move(forwardPass));
		
	}

	void GFXPassGraph::Execute()
	{
		OPTICK_EVENT();

		AssertMsg(mRenderPasses.size() > 0, "Must always have at least one render pass.");

		CD3DX12_GPU_DESCRIPTOR_HANDLE prevHandle;
		for (auto& renderPass : mRenderPasses)
		{
			renderPass->SetResourceGPUHandle(prevHandle);
			renderPass->Execute();
			U64 temp = 0;
			prevHandle = renderPass->GetResourceGPUHandle(temp);
		}
	}

}