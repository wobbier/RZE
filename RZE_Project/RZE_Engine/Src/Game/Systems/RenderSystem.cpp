#include <StdAfx.h>
#include <Game/Systems/RenderSystem.h>

#include <Game/ECS/Entity.h>
#include <Game/Components/LightSourceComponent.h>
#include <Game/Components/MeshComponent.h>
#include <Game/Components/TransformComponent.h>
#include <Game/Components/FontRenderComponent.h>

#include <RenderCore/Renderer.h>
#include <RenderCore/HardwareInterface/OpenGL.h>

#include <RenderCore/Graphics/Mesh.h>
#include <RenderCore/Graphics/Texture2D.h>

#include <RenderCore/Shaders/Shader.h>
#include <RenderCore/Shaders/ShaderGroup.h>

#include <Utils/Math/Vector4D.h>

RenderSystem::RenderSystem(IEntityAdmin* const admin)
    : IEntitySystem(admin)
{
    OpenGLRHI::Get().ClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::Init()
{
    LoadFontShader();
}

void RenderSystem::Update()
{
    RZE_Renderer* const renderer = mAdmin->GetRenderer();
    AssertNotNull(renderer);

    const std::vector<IEntity*>& entityList = mAdmin->GetEntities();
    for (auto& entity : entityList)
    {
        MeshComponent* const meshComponent = static_cast<MeshComponent* const>(entity->GetComponents()[0]);
        
        LightSourceComponent* lightComponent = nullptr;
        if (entity->GetComponents().size() == 3)
        {
            lightComponent = static_cast<LightSourceComponent*>(entity->GetComponents()[2]);
        }

        TransformComponent* const transform = static_cast<TransformComponent* const>(entity->GetComponents()[1]);
        SceneCamera& renderCam = renderer->GetSceneCamera();
        renderCam.GenerateProjectionMat();
        renderCam.GenerateViewMat();

        Matrix4x4 modelMat;
        modelMat.Translate(transform->GetPosition());
        modelMat.Rotate(transform->GetRotation().ToAngle(), transform->GetRotation().ToAxis());
        modelMat.Scale(transform->GetScale());

        Matrix4x4 MVP = renderCam.GetProjectionMat() * renderCam.GetViewMat() * modelMat;

        RZE_Renderer::RenderItemProtocol renderItem;
        renderItem.mShaderGroup = meshComponent->GetShaderGroup();
        renderItem.mModelMat = modelMat;
        renderItem.mProjectionMat = renderCam.GetProjectionMat();
        renderItem.mViewMat = renderCam.GetViewMat();
        // #TODO find a better transfer point for the resource handler. Maybe pass in as an argument to constructor for renderer?
        renderItem.mMeshData = RZE_Engine::Get()->GetResourceHandler().GetResource<MeshResource>(meshComponent->GetMeshHandle());

        if (meshComponent->GetTextureHandle().IsValid())
        {
            renderItem.mTextureData = RZE_Engine::Get()->GetResourceHandler().GetResource<GFXTexture2D>(meshComponent->GetTextureHandle());
        }
        
        if (lightComponent)
        {
            RZE_Renderer::LightItemProtocol lightItem;
            lightItem.mLightColor = lightComponent->GetColor();
            lightItem.mLightPos = transform->GetPosition();
            lightItem.mLightStrength = lightComponent->GetStrength();

            renderer->AddLightItem(lightItem);
        }

        if (renderer)
        {
            renderer->AddRenderItem(renderItem);
        }
    }
}

void RenderSystem::ShutDown()
{
}

void RenderSystem::LoadFontShader()
{
    const char* const vertFilePath = "./../RZE_Engine/Assets/Shaders/FontVert.shader";
    const char* const fragFilePath = "./../RZE_Engine/Assets/Shaders/FontFrag.shader";

    ResourceHandler& resourceHandler = RZE_Engine::Get()->GetResourceHandler();

    ResourceHandle vertShaderRes = resourceHandler.RequestResource<GFXShader>(
                                                                                                                vertFilePath, 
                                                                                                                EGLShaderType::Vertex, 
                                                                                                                "FontVertShader"
                                                                                                           );

    ResourceHandle fragShaderRes = resourceHandler.RequestResource<GFXShader>(
                                                                                                                fragFilePath,
                                                                                                                EGLShaderType::Fragment,
                                                                                                                "FontFragShader"
                                                                                                           );

    GFXShader* vertShader = resourceHandler.GetResource<GFXShader>(vertShaderRes);
    vertShader->Create();
    vertShader->Compile();

    GFXShader* fragShader = resourceHandler.GetResource<GFXShader>(fragShaderRes);
    fragShader->Create();
    fragShader->Compile();

    GFXShaderGroup* shaderGroup = new GFXShaderGroup("FontShader");
    shaderGroup->AddShader(GFXShaderGroup::EShaderIndex::Vertex, vertShader);
    shaderGroup->AddShader(GFXShaderGroup::EShaderIndex::Fragment, fragShader);

    shaderGroup->AddUniform("UProjectionMat");
    shaderGroup->AddUniform("UTextColor");

    shaderGroup->GenerateShaderProgram();

    resourceHandler.ReleaseResource(vertShaderRes);
    resourceHandler.ReleaseResource(fragShaderRes);

    mShaderGroups["FontShader"] = shaderGroup; 
}
