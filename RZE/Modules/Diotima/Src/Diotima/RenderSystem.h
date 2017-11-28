#pragma once

#include <Utils/Interfaces/SubSystem.h>

namespace Diotima
{
	class RenderSystem : public ISubSystem
	{
	public:
		RenderSystem();
		virtual ~RenderSystem();

	private:
		virtual void Initialize();
		virtual void Update();
		virtual void ShutDown();
	};
}
