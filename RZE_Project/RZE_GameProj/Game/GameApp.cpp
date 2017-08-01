#include "GameApp.h"

#include <RZE_Config.h>
#include <RZE.h>

#include <DebugUtils/Debug.h>

#include <Game/GameEntity.h>
#include <Game/Components/MeshComponent.h>
#include <Game/Components/TransformComponent.h>
#include <Game/Components/LightSourceComponent.h>

#include <Windowing/WinKeyCodes.h>

GameApp::GameApp()
    : RZE_Game()
{
}

GameApp::~GameApp()
{
}

void GameApp::RegisterEvents(EventHandler& eventHandler)
{
    static float angleX_1 = 0.0f;
    static float angleY_1 = 0.0f;
    
    static float speedX = 2.0f;
    static float speedY = 2.0f;
    static float speedZ = 4.0f;

    static float deltaT = (1.0f / 60.0f);

    Functor<void, const Event&> keyEvent([this](const Event& evt)
    {
        if (evt.mInfo.mEventType == EEventType::Key)
        {
            //
            // MainTestLight
            //
            if (evt.mKeyEvent.mKey == Win32KeyCode::Up)
            {
                TransformComponent* const transformComp = static_cast<TransformComponent* const>(mLightEntity->GetComponents()[1]);

                Vector3D currPos = transformComp->GetPosition();
                Vector3D newPos = currPos + Vector3D(0.0f, 0.0f, (-2.0f * speedY) * deltaT);

                transformComp->SetPosition(newPos);
            }
            else if (evt.mKeyEvent.mKey == Win32KeyCode::Down)
            {
                TransformComponent* const transformComp = static_cast<TransformComponent* const>(mLightEntity->GetComponents()[1]);

                Vector3D currPos = transformComp->GetPosition();
                Vector3D newPos = currPos + Vector3D(0.0f, 0.0f, (2.0f * speedY) * deltaT);

                transformComp->SetPosition(newPos);
            }
            
            if (evt.mKeyEvent.mKey == Win32KeyCode::LEFT)
            {
                TransformComponent* const transformComp = static_cast<TransformComponent* const>(mLightEntity->GetComponents()[1]);

                Vector3D currPos = transformComp->GetPosition();
                Vector3D newPos = currPos + Vector3D((-2.0f * speedX) * deltaT, 0.0f, 0.0f);

                transformComp->SetPosition(newPos);
            }
            else if (evt.mKeyEvent.mKey == Win32KeyCode::Right)
            {
                TransformComponent* const transformComp = static_cast<TransformComponent* const>(mLightEntity->GetComponents()[1]);

                Vector3D currPos = transformComp->GetPosition();
                Vector3D newPos = currPos + Vector3D((2.0f * speedX) * deltaT, 0.0f, 0.0f);

                transformComp->SetPosition(newPos);
            }

            if (evt.mKeyEvent.mKey == Win32KeyCode::OEM_Plus)
            {
                TransformComponent* const transformComp = static_cast<TransformComponent* const>(mLightEntity->GetComponents()[1]);

                Vector3D currPos = transformComp->GetPosition();
                Vector3D newPos = currPos + Vector3D(0.0f, (2.0f * speedZ) * deltaT, 0.0f);

                transformComp->SetPosition(newPos);
            }
            else if (evt.mKeyEvent.mKey == Win32KeyCode::OEM_Minus)
            {
                TransformComponent* const transformComp = static_cast<TransformComponent* const>(mLightEntity->GetComponents()[1]);

                Vector3D currPos = transformComp->GetPosition();
                Vector3D newPos = currPos + Vector3D(0.0f, (-2.0f * speedZ) * deltaT, 0.0f);

                transformComp->SetPosition(newPos);
            }
        }
    });
    eventHandler.RegisterForEvent(EEventType::Key, keyEvent);
}

void GameApp::Start()
{
    RZE_Game::Start();

    // ALL TEST CODE

    const char* const cubeFilePath = "./../RZE_Engine/Assets/3D/Cube.obj";
    const char* const miniCooperFilePath = "./../RZE_Engine/Assets/3D/MiniCooper.obj";
    const char* const lampFilePath = "./../RZE_Engine/Assets/3D/Lamp.obj";

    const char* const quadMeshFilePath = "./../RZE_Engine/Assets/3D/Quad.obj";

    ResourceHandle cubeMesh = RZE_Engine::Get()->GetResourceHandler().RequestResource<MeshResource>(cubeFilePath);
    ResourceHandle lampMesh = RZE_Engine::Get()->GetResourceHandler().RequestResource<MeshResource>(lampFilePath);

    ResourceHandle quadMesh = RZE_Engine::Get()->GetResourceHandler().RequestResource<MeshResource>(quadMeshFilePath);

    CreateLight(cubeMesh);
    CreateGround(cubeMesh);
    CreateLampObjects(lampMesh);
    CreateTextureQuad(quadMesh); 
}

void GameApp::Update()
{
    RZE_Game::Update();
}

void GameApp::CreateTextureQuad(const ResourceHandle& resourceHandle)
{
    GameEntity* entity = RZE_Engine::Get()->GetWorld()->AddEntity<GameEntity>();

    entity->AddComponent<MeshComponent>("TextureTestQuad");
    MeshComponent* const meshComp = static_cast<MeshComponent* const>(entity->GetComponents()[0]);
    meshComp->SetResource(resourceHandle);

    entity->AddComponent<TransformComponent>();
    TransformComponent* const transfComp = static_cast<TransformComponent* const>(entity->GetComponents()[1]);
    transfComp->SetPosition(Vector3D(0.0f, 2.0f, -3.0f));
    transfComp->SetScale(Vector3D(4.0f, 3.0f, 0.0f));

    mEntities.push_back(entity);
}

void GameApp::CreateLight(const ResourceHandle& resourceHandle)
{
    mLightEntity = RZE_Engine::Get()->GetWorld()->AddEntity<GameEntity>();

    mLightEntity->AddComponent<MeshComponent>();
    MeshComponent* const lightMesh = static_cast<MeshComponent* const>(mLightEntity->GetComponents()[0]);
    lightMesh->SetResource(resourceHandle);

    mLightEntity->AddComponent<TransformComponent>();
    TransformComponent* const lightTransform = static_cast<TransformComponent* const>(mLightEntity->GetComponents()[1]);
    lightTransform->SetPosition(Vector3D(-8.0f, 15.0f, -2.0f));

    mLightEntity->AddComponent<LightSourceComponent>("MainTestLight");
    LightSourceComponent* const lightComponent = static_cast<LightSourceComponent* const>(mLightEntity->GetComponents()[2]);
    lightComponent->SetColor(Vector3D(1.0f, 1.0f, 1.0f));
    lightComponent->SetStrength(0.65f);

    mEntities.push_back(mLightEntity);
}

void GameApp::CreateGround(const ResourceHandle& resourceHandle)
{
    const int numEntities = 1;
    for (int entIdx = 0; entIdx < numEntities; ++entIdx)
    {
        GameEntity* testEntity = RZE_Engine::Get()->GetWorld()->AddEntity<GameEntity>();

        testEntity->AddComponent<MeshComponent>();
        MeshComponent* const meshComponent = static_cast<MeshComponent* const>(testEntity->GetComponents()[0]);
        meshComponent->SetResource(resourceHandle);

        testEntity->AddComponent<TransformComponent>();
        TransformComponent* const transformComp = static_cast<TransformComponent* const>(testEntity->GetComponents()[1]);
        transformComp->SetPosition(Vector3D(-10.0f, -5.0f, -20.0f));
        transformComp->SetRotation(Quaternion(Vector3D(0.0f, 0.0f, 0.0f)));
        transformComp->SetScale(Vector3D(20.0f, 0.5f, 20.0f));

        mEntities.push_back(testEntity);
    }
}

void GameApp::CreateLampObjects(const ResourceHandle& resourceHandle)
{

    //
    // 1 - NE
    //
    GameEntity* lampEntity = RZE_Engine::Get()->GetWorld()->AddEntity<GameEntity>();

    lampEntity->AddComponent<MeshComponent>("LampMesh");
    MeshComponent* meshComponent = static_cast<MeshComponent*>(lampEntity->GetComponents()[0]);
    meshComponent->SetResource(resourceHandle);

    lampEntity->AddComponent<TransformComponent>();
    TransformComponent* transformComp = static_cast<TransformComponent*>(lampEntity->GetComponents()[1]);
    transformComp->SetPosition(Vector3D(-8.0f, -3.0f, -15.0f));
    transformComp->SetScale(Vector3D(0.5f, 0.5f, 0.5f));

    mEntities.push_back(lampEntity);

    //
    // 2 - NW
    //
    lampEntity = RZE_Engine::Get()->GetWorld()->AddEntity<GameEntity>();

    lampEntity->AddComponent<MeshComponent>("LampMesh");
    meshComponent = static_cast<MeshComponent* const>(lampEntity->GetComponents()[0]);
    meshComponent->SetResource(resourceHandle);

    lampEntity->AddComponent<TransformComponent>();
    transformComp = static_cast<TransformComponent* const>(lampEntity->GetComponents()[1]);
    transformComp->SetPosition(Vector3D(8.0f, -3.0f, -15.0f));
    transformComp->SetScale(Vector3D(0.5f, 0.5f, 0.5f));

    mEntities.push_back(lampEntity);

    //
    // 3 - SE
    //
    lampEntity = RZE_Engine::Get()->GetWorld()->AddEntity<GameEntity>();

    lampEntity->AddComponent<MeshComponent>("LampMesh");
    meshComponent = static_cast<MeshComponent* const>(lampEntity->GetComponents()[0]);
    meshComponent->SetResource(resourceHandle);

    lampEntity->AddComponent<TransformComponent>();
    transformComp = static_cast<TransformComponent* const>(lampEntity->GetComponents()[1]);
    transformComp->SetPosition(Vector3D(-8.0f, -3.0f, -2.0f));
    transformComp->SetScale(Vector3D(0.5f, 0.5f, 0.5f));

    mEntities.push_back(lampEntity);

    //
    // 4 - SW
    //
    lampEntity = RZE_Engine::Get()->GetWorld()->AddEntity<GameEntity>();

    lampEntity->AddComponent<MeshComponent>("LampMesh");
    meshComponent = static_cast<MeshComponent* const>(lampEntity->GetComponents()[0]);
    meshComponent->SetResource(resourceHandle);

    lampEntity->AddComponent<TransformComponent>();
    transformComp = static_cast<TransformComponent* const>(lampEntity->GetComponents()[1]);
    transformComp->SetPosition(Vector3D(8.0f, -3.0f, -2.0f));
    transformComp->SetScale(Vector3D(0.5f, 0.5f, 0.5f));

    mEntities.push_back(lampEntity);
}
