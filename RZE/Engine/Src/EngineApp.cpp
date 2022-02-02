#include <StdAfx.h>
#include <EngineApp.h>

#include <EngineCore/Engine.h>

#include <Game/World/GameObjectComponents/CameraComponent.h>
#include <Game/World/GameObjectComponents/MeshComponent.h>
#include <Game/World/GameObjectComponents/TransformComponent.h>

#include <Windowing/Win32Window.h>

#include <Rendering/Driver/DX11/DX11Device.h>
#include <Rendering/Graphics/RenderTarget.h>

#include <Utils/DebugUtils/Debug.h>

RZE_Engine RZE_Application::mEngine;

RZE_Application::RZE_Application()
	: mWindow(nullptr)
{
	SetRunning(false);
}

RZE_Application::~RZE_Application()
{

}

void RZE_Application::Run()
{
	SetRunning(true);

	Functor<RZE_Application* const> engineHook([this]()
	{
		return this;
	});

	RZE().Run(engineHook);
}

void RZE_Application::Start()
{
	
}

void RZE_Application::Update()
{
}

void RZE_Application::ShutDown()
{

}

bool RZE_Application::ProcessInput(const InputHandler& handler)
{
	return true;
}

void RZE_Application::RegisterInputEvents(InputHandler& inputHandler)
{
}

void RZE_Application::Initialize()
{
	LOG_CONSOLE("RZE_Application::Init() called.");

	// #TODO This basically just runs a bunch of poor mans reflection stuff like
	// creating entries in the component name lookup and factory functions. Necessary to function atm.
	// Eventually hope to implement actual reflection and handle this.
	REGISTER_GAMEOBJECTCOMPONENT(CameraComponent);
	REGISTER_GAMEOBJECTCOMPONENT(MeshComponent);
	REGISTER_GAMEOBJECTCOMPONENT(TransformComponent);
}

Win32Window& RZE_Application::InternalGetWindow()
{
	AssertNotNull(mWindow);
	return *mWindow;
}

void RZE_Application::SetRunning(bool bRunning)
{
	bIsRunning = bRunning;
}

void RZE_Application::ShowWindow()
{
	AssertNotNull(mWindow);
	mWindow->Show();
}

void RZE_Application::SetWindow(Win32Window* const window)
{
	AssertNotNull(window);
	mWindow = window;
}

Win32Window* const RZE_Application::GetWindow() const
{
	return mWindow;
}

Rendering::RenderTargetTexture* RZE_Application::GetRTT()
{
	AssertNotNull(mRenderTarget);
	return mRenderTarget.get();
}

void RZE_Application::ParseArguments(const char* arguments, int count)
{
}
