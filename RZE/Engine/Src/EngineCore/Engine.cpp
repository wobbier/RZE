#include <StdAfx.h>
#include <EngineCore/Engine.h>

#include <DebugUtils/DebugServices.h>

#include <Diotima/Renderer.h>
#include <Diotima/Graphics/RenderTarget.h>

#include <ECS/Components/CameraComponent.h>
#include <ECS/Components/LightSourceComponent.h>
#include <ECS/Components/MaterialComponent.h>
#include <ECS/Components/MeshComponent.h>
#include <ECS/Components/NameComponent.h>
#include <ECS/Components/TransformComponent.h>

#include <ECS/Systems/RenderSystem.h>

#include <Windowing/Win32Window.h>
#include <Windowing/WinKeyCodes.h>

#include <Utils/DebugUtils/Debug.h>

RZE_Engine::RZE_Engine()
	: mMainWindow(nullptr)
	, mEngineConfig(nullptr)
	, mApplication(nullptr)
	, mRenderer(nullptr)
	, bShouldExit(false)
	, bIsInitialized(false)
	, mDeltaTime(0.0)
	, mFrameCount(0)
{
	mFrameSamples.resize(MAX_FRAMETIME_SAMPLES);
}

RZE_Engine::~RZE_Engine()
{
}

void RZE_Engine::Run(Functor<RZE_Application* const>& createApplicationCallback)
{
	Init();

	if (bIsInitialized)
	{
		AssertNotNull(createApplicationCallback);
		PostInit(createApplicationCallback);

		HiResTimer programTimer;
		programTimer.Start();
		double prevTime = programTimer.GetElapsed<double>();
		while (!bShouldExit)
		{
			BROFILER_FRAME("Engine Thread");

			double currTime = programTimer.GetElapsed<double>();
			double frameTime = currTime - prevTime;
			prevTime = currTime;

			mDeltaTime = frameTime;
			mFrameSamples[mFrameCount % MAX_FRAMETIME_SAMPLES] = static_cast<float>(mDeltaTime);

			const float averageFrametime = CalculateAverageFrametime();

			{	BROFILER_CATEGORY("RZE_Engine::Run", Profiler::Color::Cyan)

				PreUpdate();
				{
					Update();
					mRenderer->Update();
					mRenderer->Render();
				}
			}

			++mFrameCount;

			{
				BROFILER_CATEGORY("BufferSwap", Profiler::Color::Aquamarine);
				mMainWindow->BufferSwap(); // #TODO(Josh) Maybe this can be done better
			}
		}

		BeginShutDown();
	}
	else
	{
		LOG_CONSOLE("Initialization Failed.");
	}
}

const Vector2D& RZE_Engine::GetWindowSize() const
{
	AssertNotNull(mMainWindow);
	return mMainWindow->GetDimensions();
}

RZE_Application& RZE_Engine::GetApplication()
{
	AssertNotNull(mApplication);
	return *mApplication;
}

void RZE_Engine::Init()
{
	if (!IsInitialized())
	{
		LOG_CONSOLE("RZE_EngineCore::Init() called.");

		Perseus::JobScheduler::Get().Initialize();

		LoadEngineConfig();

		CreateAndInitializeWindow();

		mInputHandler.Initialize();

		RegisterWindowEvents();
		RegisterEngineComponentTypes();

		mResourceHandler.Init();

		CreateAndInitializeRenderer();

		mActiveScene = new GameScene();
		mActiveScene->Initialize();

		bIsInitialized = true;
	}
}

void RZE_Engine::PostInit(Functor<RZE_Application* const>& createApplicationCallback)
{
	LOG_CONSOLE("RZE_EngineCore::PostInit() called.");

	InitializeApplication(createApplicationCallback);

	mActiveScene->Start();
}

void RZE_Engine::PreUpdate()
{	BROFILER_CATEGORY("RZE_Engine::PreUpdate", Profiler::Color::Purple)
	CompileEvents();
	mEventHandler.ProcessEvents();

	if (mApplication->ProcessInput(mInputHandler))
	{
		mInputHandler.RaiseEvents();
	}
	else
	{
		mInputHandler.Reset();
	}
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

	mMainWindow->RegisterEvents(mEventHandler);

	mMainWindow->Show();
}

void RZE_Engine::CreateAndInitializeRenderer()
{
	mRenderer = new Diotima::Renderer();

	mRenderer->SetWindow(mMainWindow->GetOSWindowHandleData().windowHandle);

	mRenderer->SetMSAASampleCount(mEngineConfig->GetEngineSettings().GetMSAASampleCount());
	mRenderer->Initialize();
	mRenderer->EnableVsync(mEngineConfig->GetEngineSettings().IsVSyncEnabled());
}

void RZE_Engine::InitializeApplication(Functor<RZE_Application* const> createGameCallback)
{
	mApplication = createGameCallback();
	AssertNotNull(mApplication);

	mApplication->Initialize();
	mApplication->RegisterInputEvents(mInputHandler);
	mApplication->SetWindow(mMainWindow);

	mApplication->Start();
}

float RZE_Engine::CalculateAverageFrametime()
{
	float sum = 0.0f;
	for (float frameSample : mFrameSamples)
	{
		sum += frameSample;
	}

	sum /= MAX_FRAMETIME_SAMPLES;
	return sum;
}


void RZE_Engine::SetWindowSize(Vector2D newSize)
{
	AssertNotNull(mMainWindow);
	mMainWindow->SetWindowSize(newSize);
}

void RZE_Engine::CompileEvents()
{
	mMainWindow->CompileInputMessages(mInputHandler);
	mMainWindow->CompileWindowMessages(mEventHandler);
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
		else if (event.mWindowEvent.mEventInfo.mEventSubType == EWindowEventType::Window_Resize)
		{
			Vector2D newSize(event.mWindowEvent.mSizeX, event.mWindowEvent.mSizeY);
			GetRenderer().ResizeCanvas(newSize);
		}
	});
	mEventHandler.RegisterForEvent(EEventType::Window, windowCallback);
}

void RZE_Engine::RegisterEngineComponentTypes()
{
	APOLLO_REGISTER_COMPONENT(CameraComponent);
	APOLLO_REGISTER_COMPONENT(LightSourceComponent);
	APOLLO_REGISTER_COMPONENT(MaterialComponent);
	APOLLO_REGISTER_COMPONENT(MeshComponent);
	APOLLO_REGISTER_COMPONENT(NameComponent);
	APOLLO_REGISTER_COMPONENT(TransformComponent);
}

void RZE_Engine::LoadEngineConfig()
{
	// #TODO(Josh) This should probably be a resource? 
	mEngineConfig = new EngineConfig();
	mEngineConfig->Load(FilePath("Config/Engine.ini"));

	if (mEngineConfig->Empty())
	{
		LOG_CONSOLE("Engine config could not load. Defaults were used.");
	}
}

void RZE_Engine::Update()
{	BROFILER_CATEGORY("RZE_Engine::Update", Profiler::Color::Orchid)
	mActiveScene->Update();
	mApplication->Update();
}

void RZE_Engine::BeginShutDown()
{
	LOG_CONSOLE("Shutting engine down...");
	
	mActiveScene->Finish();
	mApplication->ShutDown();
	mResourceHandler.ShutDown();

	Perseus::JobScheduler::Get().ShutDown();

	InternalShutDown();
}

void RZE_Engine::InternalShutDown()
{
	AssertNotNull(mMainWindow);
	AssertNotNull(mEngineConfig);
	AssertNotNull(mRenderer);
	AssertNotNull(mActiveScene);

	delete mMainWindow;
	delete mEngineConfig;
	delete mRenderer;
	delete mActiveScene;
}

void RZE_Engine::PostExit()
{
	bShouldExit = true;
}

ResourceHandler& RZE_Engine::GetResourceHandler()
{
	return mResourceHandler;
}

InputHandler& RZE_Engine::GetInputHandler()
{
	return mInputHandler;
}

Diotima::Renderer& RZE_Engine::GetRenderer()
{
	AssertNotNull(mRenderer);
	return *mRenderer;
}

GameScene& RZE_Engine::GetActiveScene()
{
	AssertNotNull(mActiveScene);
	return *mActiveScene;
}

void RZE_Engine::Log(const std::string& text, const Vector3D& color)
{
}

