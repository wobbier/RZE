#include "StdAfx.h"

#include "Engine.h"

#include "Debug/Debug.h"

#include "Game/GameWorld.h"

#include "Windowing/Win32Window.h"
#include "Windowing/WinKeyCodes.h"

UInt8 RZE_Engine::sInstanceCount = 0;

RZE_Engine::RZE_Engine()
	: bShouldExit(false)
	, mMainWindow(nullptr)
{
	++sInstanceCount;
	AssertMsg(sInstanceCount == 1, "Trying to create second engine class, invalid operation. Exiting.");
	if (sInstanceCount != 1)
	{
		exit(0);
	}

	Init();
}

RZE_Engine::~RZE_Engine()
{
	AssertNotNull(mMainWindow);
	delete mMainWindow;
}

void RZE_Engine::Run(Functor<RZE_Game* const> createGameCallback)
{
	AssertNotNull(createGameCallback);
	PostInit(createGameCallback);

	while (!bShouldExit)
	{
		Update();
	}

	BeginShutDown();
}

void RZE_Engine::Init()
{
	LOG_CONSOLE("RZE_EngineCore::Init() called.");

	CreateAndInitializeWindow();

	mRenderer.Init();

	RegisterWindowEvents();
	RegisterInputEvents();

	mInputHandler.RegisterEvents(mEventHandler);
}

void RZE_Engine::InitWorld()
{
	LOG_CONSOLE_ANNOUNCE("Initializing Game World...");
	// @note naieve at first
	mWorld = new GameWorld();
	AssertNotNull(mWorld);

	mWorld->InitSystems();
}

void RZE_Engine::PostInit(Functor<RZE_Game* const> createApplicationCallback)
{
	LOG_CONSOLE("RZE_EngineCore::PostInit() called.");

	InitWorld();
	InitGame(createApplicationCallback);
}

void RZE_Engine::CreateAndInitializeWindow()
{
	Win32Window::WindowCreationParams params;
	params.windowTitle = "test";
	params.width = 1280;
	params.height = 720;

	mMainWindow = new Win32Window(params);
	AssertNotNull(mMainWindow);
}

void RZE_Engine::InitGame(Functor<RZE_Game* const> createGameCallback)
{
	mApplication = createGameCallback();
	AssertNotNull(mApplication);

	mApplication->SetWindow(mMainWindow);
	mApplication->Start();

	mApplication->RegisterEvents(mEventHandler);
}

void RZE_Engine::CompileEvents()
{
	mApplication->CompileEvents(mEventHandler);
}

void RZE_Engine::RegisterWindowEvents()
{
	Functor<void, const Event&> windowCallback([this](const Event& event)
	{
		AssertEqual(event.mInfo.mEventType, EEventType::Window);
		if (event.mWindowEvent.mEventInfo.mEventSubType == EWindowEventType::Window_Destroy)
		{
			this->bShouldExit = true;
		}
	});

	RegisterForEvent(EEventType::Window, windowCallback);
}

void RZE_Engine::RegisterInputEvents()
{
	Functor<void, UInt8> keyPressCallback([this](const UInt8 key)
	{
		if (key == Win32KeyCode::Escape)
		{
			this->bShouldExit = true;
		}
	});

	mInputHandler.RegisterForEvent(EKeyEventType::Key_Pressed, keyPressCallback);
}

void RZE_Engine::Update()
{
	CompileEvents();
	mEventHandler.ProcessEvents();
	mInputHandler.ProcessEvents();

	mApplication->Update();
	mWorld->Update();

	// @todo maybe this can be done better
	mRenderer.Render();
	mMainWindow->BufferSwap();
}

void RZE_Engine::BeginShutDown()
{
	LOG_CONSOLE("Shutting engine down...");
}

void RZE_Engine::RegisterForEvent(const UInt16 eventType, Functor<void, const Event&> callback)
{
	mEventHandler.RegisterForEvent(eventType, callback);
}

GameWorld* const RZE_Engine::GetWorld() const
{
	AssertNotNull(mWorld);
	return mWorld;
}
