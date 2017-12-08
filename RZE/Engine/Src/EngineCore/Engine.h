#pragma once

#include <EngineApp.h>

#include <RZE_Config.h>

#include <Utils/Platform/File.h>

// @todo:josh this is where the weird include-before issue is
#include <Events/EventHandler.h>

#include <EngineCore/SubSystemHandler.h>
#include <EngineCore/Config/EngineConfig.h>
#include <EngineCore/Input/InputHandler.h>
#include <EngineCore/Resources/ResourceHandler.h>

#include <Apollo/ComponentHandler.h>

#include <Diotima/Renderer.h>

class GameWorld;
class Win32Window;

namespace Apollo
{
	class EntityComponentSystem;
}

class RZE_Engine
{
	static RZE_Engine* sInstance;

public:

	RZE_Engine();
	~RZE_Engine();
	
	static RZE_Engine* const Get()
	{
		if (!sInstance)
		{
			sInstance = new RZE_Engine();
		}

		return sInstance;
	}

	inline bool IsInitialized() { return bIsInitialized; }

	void Run(Functor<RZE_Game* const>& createApplicationCallback);

	const Vector2D& GetWindowSize() const;

	ResourceHandler&	GetResourceHandler();

	// #TODO(Josh) Totally test. This is not good.
	Diotima::Renderer* GetRenderSystem();

	Apollo::ComponentHandler&	GetComponentHandler() { return mComponentHandler; }

private:

	void Init();
	void InitWorld();
	void PostInit(Functor<RZE_Game* const>& createApplicationCallback);

	void Update();

	void BeginShutDown();
	void InternalShutDown();
	void PostExit();

	void CompileEvents();

	void RegisterSubSystems();
	void RegisterWindowEvents();
	void RegisterInputEvents();

	void LoadEngineConfig();
	void CreateAndInitializeWindow();

	void InitGame(Functor<RZE_Game* const> createGameCallback);

private:
	SubSystemHandler mSubSystemHandler;

private:
	RZE_Game* mApplication;
	GameWorld* mWorld;

	Win32Window* mMainWindow;

	ResourceHandler mResourceHandler;
	EventHandler mEventHandler;
	InputHandler mInputHandler;
	Apollo::ComponentHandler mComponentHandler;

	EngineConfig* mEngineConfig;

	bool bIsInitialized;
	bool bShouldExit;
};
