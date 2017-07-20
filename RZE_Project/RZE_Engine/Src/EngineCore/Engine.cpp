#include <StdAfx.h>

#include <EngineCore/Engine.h>

#include <DebugUtils/Debug.h>

#include <Game/GameWorld.h>

#include <RenderCore/Renderer.h>
#include <RenderCore/HardwareInterface/OpenGL.h>

#include <Windowing/Win32Window.h>
#include <Windowing/WinKeyCodes.h>

RZE_Engine* RZE_Engine::sInstance = nullptr;

RZE_Engine::RZE_Engine()
{
    mMainWindow         = nullptr;
    mRenderer           = nullptr;
    mWorld              = nullptr;
    mEngineConfig       = nullptr;
    mApplication        = nullptr;

    bShouldExit         = false;
    bIsInitialized      = false;
}

RZE_Engine::~RZE_Engine()
{
    AssertNotNull(mMainWindow);
    delete mMainWindow;
}

void RZE_Engine::Run(Functor<RZE_Game* const>& createGameCallback)
{
    Init();

    if (bIsInitialized)
    {
        AssertNotNull(createGameCallback);
        PostInit(createGameCallback);

        while (!bShouldExit)
        {
            Update();
        }

        BeginShutDown();
    }
}

void RZE_Engine::Init()
{
    LOG_CONSOLE("RZE_EngineCore::Init() called.");

    LoadEngineConfig();

    CreateAndInitializeWindow();

    {
        OpenGLRHI::OpenGLCreationParams creationParams;
        creationParams.WindowWidth = static_cast<int>(mMainWindow->GetDimensions().X());
        creationParams.WindowHeight = static_cast<int>(mMainWindow->GetDimensions().Y());

        OpenGLRHI::Get().Init(creationParams);
    }

    // #TODO this should be handled better. No need to create directly here. Take a look.
    mRenderer = new RZE_Renderer();

    RegisterWindowEvents();
    RegisterInputEvents();

    mInputHandler.RegisterEvents(mEventHandler);

    bIsInitialized = true;
}

void RZE_Engine::InitWorld()
{
    LOG_CONSOLE_ANNOUNCE("Initializing Game World...");

    // @note naive at first
    mWorld = new GameWorld(mRenderer);
    AssertNotNull(mWorld);

    mWorld->Init();
}

void RZE_Engine::PostInit(Functor<RZE_Game* const>& createApplicationCallback)
{
    LOG_CONSOLE("RZE_EngineCore::PostInit() called.");

    InitWorld();
    InitGame(createApplicationCallback);
}

void RZE_Engine::CreateAndInitializeWindow()
{
    WindowSettings& windowSettings = mEngineConfig->GetWindowSettings();

    Win32Window::WindowCreationParams params;
    params.windowTitle = windowSettings.GetTitle();
    params.width = static_cast<int>(windowSettings.GetDimensions().X());
    params.height = static_cast<int>(windowSettings.GetDimensions().Y());

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
            PostExit();
        }
    });
    RegisterForEvent(EEventType::Window, windowCallback);
}

void RZE_Engine::RegisterInputEvents()
{
    Functor<void, U8> keyPressCallback([this](const U8 key)
    {
        if (key == Win32KeyCode::Escape)
        {
            PostExit();
        }
    });
    RegisterForInputEvent(EKeyEventType::Key_Pressed, keyPressCallback);
}

void RZE_Engine::LoadEngineConfig()
{
    mEngineConfig = new EngineConfig();
    //#TODO the path here needs to be dealt with properly
    mEngineConfig->Load("./../RZE_Engine/RZE/Config/Engine.ini");

    if (mEngineConfig->Empty())
    {
        LOG_CONSOLE("Engine config could not load. Defaults were used.");
    }
}

void RZE_Engine::Update()
{
    CompileEvents();
    mEventHandler.ProcessEvents();
    mInputHandler.ProcessEvents();

    mApplication->Update();
    mWorld->Update();

    mRenderer->Render();
    // @todo maybe this can be done better
    mMainWindow->BufferSwap();
}

void RZE_Engine::BeginShutDown()
{
    LOG_CONSOLE("Shutting engine down...");
    mWorld->ShutDown();
    mApplication->ShutDown();
}

void RZE_Engine::PostExit()
{
    bShouldExit = true;
}

void RZE_Engine::RegisterForEvent(const U16 eventType, Functor<void, const Event&> callback)
{
    mEventHandler.RegisterForEvent(eventType, callback);
}

void RZE_Engine::RegisterForInputEvent(const U16 eventType, Functor<void, U8> callback)
{
    mInputHandler.RegisterForEvent(eventType, callback);
}

WindowSettings& RZE_Engine::GetWindowSettings()
{
    return mEngineConfig->GetWindowSettings();
}

GameWorld* const RZE_Engine::GetWorld() const
{
    AssertNotNull(mWorld);
    return mWorld;
}

ResourceHandler& RZE_Engine::GetResourceHandler()
{
    return mResourceHandler;
}
