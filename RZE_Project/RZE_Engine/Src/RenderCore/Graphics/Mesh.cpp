#include <StdAfx.h>
#include <RenderCore/Graphics/Mesh.h>

#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>
#include <Assimp/scene.h>

GFXMesh::GFXMesh()
{
}

GFXMesh::~GFXMesh()
{
}

MeshData::MeshData()
{
}

MeshData::~MeshData()
{
}

void MeshData::LoadFromFile(const std::string& filePath)
{
    Assimp::Importer ModelImporter;
    const aiScene* AssimpScene = ModelImporter.ReadFile(filePath,
                                                            aiProcess_Triangulate 
                                                        |   aiProcess_GenNormals);

    bool bAssimpNotLoaded =
        !AssimpScene ||
        !AssimpScene->mRootNode ||
        AssimpScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE;

    if (bAssimpNotLoaded)
    {
        // #TODO More informative error message.
        LOG_CONSOLE("Failed to load Assimp.");
        return;
    }

    ProcessNode(*AssimpScene->mRootNode, *AssimpScene);

    if (mMeshList.size() != AssimpScene->mNumMeshes)
    {
        // #TODO More informative error message.
        LOG_CONSOLE("Error reading meshes.");
        return;
    }
}

void MeshData::ProcessNode(const aiNode& node, const aiScene& scene)
{
    for (U32 meshIdx = 0; meshIdx < node.mNumMeshes; meshIdx++)
    {
        const unsigned int assimpMeshIdx = node.mMeshes[meshIdx];
        const aiMesh& assimpMesh = *scene.mMeshes[assimpMeshIdx];

        GFXMesh* Mesh = new GFXMesh();
        ProcessMesh(assimpMesh, scene, *Mesh);
        mMeshList.push_back(Mesh);
    }

    for (U32 childNodeIdx = 0; childNodeIdx < node.mNumChildren; childNodeIdx++)
    {
        ProcessNode(*node.mChildren[childNodeIdx], scene);
    }
}

void MeshData::ProcessMesh(const aiMesh& mesh, const aiScene& scene, GFXMesh& outMesh)
{
    for (U32 vertexIdx = 0; vertexIdx < mesh.mNumVertices; vertexIdx++)
    {
        const aiVector3D& assimpVert = mesh.mVertices[vertexIdx];
        const aiVector3D& assimpNormal = mesh.mNormals[vertexIdx];

        Vector3D vertPos(assimpVert.x, assimpVert.y, assimpVert.z);
        Vector3D vertNormal(assimpNormal.x, assimpNormal.y, assimpNormal.z);

        GFXVertex vertex;
        vertex.Position = vertPos;
        vertex.Normal = vertNormal;

        outMesh.mVertices.push_back(vertex);
    }

    for (U32 faceIdx = 0; faceIdx < mesh.mNumFaces; faceIdx++)
    {
        const aiFace& assimpFace = mesh.mFaces[faceIdx];
        for (U32 indexIdx = 0; indexIdx < assimpFace.mNumIndices; indexIdx++)
        {
            outMesh.mIndices.push_back(assimpFace.mIndices[indexIdx]);
        }
    }
}
