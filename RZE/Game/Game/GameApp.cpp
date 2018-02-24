#include "GameApp.h"

#include <RZE_Config.h>
#include <RZE.h>

#include <Utils/DebugUtils/Debug.h>
#include <Utils/Math/Math.h>
#include <Utils/Platform/FilePath.h>

// TEST
#include <Diotima/Graphics/Texture2D.h>

#include <Ecs/Components/CameraComponent.h>
#include <ECS/Components/LightSourceComponent.h>
#include <ECS/Components/MeshComponent.h>
#include <ECS/Components/TransformComponent.h>
#include <ECS/Components/MaterialComponent.h>

#include <Game/Systems/WeirdTempInputSystem.h>

GameApp::GameApp()
	: RZE_Game()
{
}

GameApp::~GameApp()
{
}

void GameApp::Start()
{
	RZE_Game::Start();

	GameScene& scene = RZE_Engine::Get()->GetActiveScene();

	// ALL TEST CODE
	scene.GetEntityHandler().AddSystem<WeirdTempInputSystem>();
	
	Apollo::EntityID floor = scene.GetEntityHandler().CreateEntity();
	scene.GetEntityHandler().AddComponent<MeshComponent>(floor, FilePath("Engine/Assets/3D/Cube.obj"));
	scene.GetEntityHandler().AddComponent<TransformComponent>(floor, Vector3D(-5.0f, -5.5f, -5.0f), Quaternion(), Vector3D(10.0f, 0.5f, 10.0f));

	// Leaving the for loop for testing purposes
	Apollo::EntityID nanosuit;
	for (int i = 0; i < 1; ++i)
	{
		nanosuit = scene.GetEntityHandler().CreateEntity();

		scene.GetEntityHandler().AddComponent<MeshComponent>(nanosuit, FilePath("Engine/Assets/3D/Nanosuit/Nanosuit.obj"));
		scene.GetEntityHandler().AddComponent<TransformComponent>(nanosuit, Vector3D(0.0f, -5.0f, 0.0f), Quaternion(), Vector3D(0.5f));
	}

	Apollo::EntityID lightSource = scene.GetEntityHandler().CreateEntity();
	scene.GetEntityHandler().AddComponent<MeshComponent>(lightSource, FilePath("Engine/Assets/3D/Cube.obj"));
	scene.GetEntityHandler().AddComponent<LightSourceComponent>(lightSource, Vector3D(0.8f, 0.8f, 0.8f), 1.5f);
	scene.GetEntityHandler().AddComponent<TransformComponent>(lightSource, Vector3D(0.0f, 2.0f, 6.0f), Quaternion(), Vector3D(1.0f));

	Apollo::EntityID camera = scene.GetEntityHandler().CreateEntity();
	scene.GetEntityHandler().AddComponent<CameraComponent>(camera);
	CameraComponent* const camComp = scene.GetEntityHandler().GetComponent<CameraComponent>(camera);
	camComp->Position = Vector3D(-4.0f, 3.0f, 8.0f);
	camComp->FOV = 45;
	camComp->NearCull = 0.1f;
	camComp->FarCull = 1000.0f;
	camComp->Forward = Vector3D(0.5f, -0.5f, -1.0f);
	camComp->UpDir = Vector3D(0.0f, 1.0f, 0.0f);
	camComp->AspectRatio = RZE_Engine::Get()->GetWindowSize().X() / RZE_Engine::Get()->GetWindowSize().Y();

	InputHandler::KeyActionFunc tempKeyFunc([this, nanosuit, &scene](const InputKey& key)
	{
		scene.GetEntityHandler().DestroyEntity(nanosuit);
	});
	RZE_Engine::Get()->GetInputHandler().BindAction(Win32KeyCode::Key_5, EButtonState::ButtonState_Pressed, tempKeyFunc);
}

void GameApp::Update()
{
	RZE_Game::Update();
}