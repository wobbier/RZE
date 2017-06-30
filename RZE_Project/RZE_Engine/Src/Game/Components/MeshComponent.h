#pragma once

#include <Game/ECS/EntityComponent.h>

class MeshData;
class GFXShaderGroup;
class OpenGLVAO;
class OpenGLVBO;
class OpenGLEBO;

class MeshComponent : public IEntityComponent
{
public:
    MeshComponent();
    virtual ~MeshComponent();

    std::vector<float>& GetVertexList();
    void SetMeshData(MeshData* const meshData);

    // @todo this will be pulled out eventually and handled better.. or should be lol
    GFXShaderGroup* const GetShaderGroup();
    void SetShaderGroup(GFXShaderGroup* const shaderGroup);

    MeshData* const GetMeshData() const;

private:

    // @todo:josh temp pointer for now because we have some cyclical issues in headers
    //            that needs to be worked out for now... 
    MeshData* mMeshData;

    GFXShaderGroup* mShaderGroup;
    std::vector<float> mVertexList;
};
