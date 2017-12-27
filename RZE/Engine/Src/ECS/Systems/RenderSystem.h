#pragma once

#include <Apollo/ECS/EntitySystem.h>

#include <Utils/Functor.h>

struct CameraComponent;

namespace Apollo
{
	class ComponentHandler;
}

class RenderSystem : public Apollo::EntitySystem
{
public:
	RenderSystem();

public:
	virtual void Initialize();
	virtual void Update(std::vector<Apollo::EntityID>& entities);
	virtual void ShutDown();

	void RegisterForComponentNotifications();

	void GenerateCameraMatrices();

private:
	std::unordered_map<Apollo::EntityID, Int32> mRenderItemEntityMap;
	std::unordered_map<Apollo::EntityID, Int32> mLightItemEntityMap;

	Diotima::GLRenderTargetTexture mRenderTargetTexture;
	CameraComponent* mMainCamera;
};