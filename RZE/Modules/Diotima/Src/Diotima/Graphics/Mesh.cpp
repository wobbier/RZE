#include <Diotima/Graphics/Mesh.h>

#include <Assimp/Importer.hpp>
#include <Assimp/postprocess.h>
#include <Assimp/scene.h>

#include <Diotima/Driver/OpenGL.h>

#include <Utils/DebugUtils/Debug.h>

namespace Diotima
{
	GFXMesh::GFXMesh()
	{
		mVAO.Init();
		mVAO.Bind();

		mVertexVBO.Init();
		mVertexVBO.Bind();

		mNormalVBO.Init();
		mNormalVBO.Bind();

		mEBO.Init();
		mEBO.Bind();
	}

	GFXMesh::~GFXMesh()
	{
	}

	OpenGLVAO& GFXMesh::GetVAO()
	{
		return mVAO;
	}

	OpenGLVBO& GFXMesh::GetVertexVBO()
	{
		return mVertexVBO;
	}

	OpenGLEBO& GFXMesh::GetEBO()
	{
		return mEBO;
	}

	const std::vector<GFXVertex>& GFXMesh::GetVertexList() const
	{
		return mVertices;
	}

	const std::vector<U32>& GFXMesh::GetIndices() const
	{
		return mIndices;
	}

	void GFXMesh::OnLoadFinished()
	{
		for (size_t vertIdx = 0; vertIdx < mVertices.size(); vertIdx++)
		{
			mPositions.push_back(mVertices[vertIdx].Position);
			mNormals.push_back(mVertices[vertIdx].Normal);

			mUVCoords.push_back(mVertices[vertIdx].UVData);
		}

		const GLsizeiptr verticesSize = mPositions.size() * sizeof(Vector3D);
		const GLsizeiptr normalsSize = mNormals.size() * sizeof(Vector3D);
		const GLsizeiptr uvDataSize = mUVCoords.size() * sizeof(Vector2D);
		const GLsizeiptr totalSize = verticesSize + normalsSize + uvDataSize;

		const GLsizeiptr normalsStart = verticesSize;
		const GLsizeiptr uvDataStart = normalsStart + normalsSize;

		const GLvoid* normalsStartPtr = reinterpret_cast<GLvoid*>(normalsStart);
		const GLvoid* uvDataStartPtr = reinterpret_cast<GLvoid*>(uvDataStart);

		mVAO.Bind();
		mVertexVBO.SetBufferData(nullptr, totalSize);
		mVertexVBO.SetBufferSubData(mPositions.data(), 0, verticesSize);
		mVertexVBO.SetBufferSubData(mNormals.data(), normalsStart, normalsSize);
		mVertexVBO.SetBufferSubData(mUVCoords.data(), uvDataStart, uvDataSize);

		mEBO.SetBufferData(mIndices.data(), sizeof(U32) * mIndices.size());

		// vertices
		OpenGLRHI::Get().EnableVertexAttributeArray(0);
		OpenGLRHI::Get().VertexAttribPointer(0, 3, EGLDataType::Float, EGLBooleanValue::False, sizeof(Vector3D), nullptr);

		// normals
		OpenGLRHI::Get().EnableVertexAttributeArray(1);
		OpenGLRHI::Get().VertexAttribPointer(1, 3, EGLDataType::Float, EGLBooleanValue::False, sizeof(Vector3D), normalsStartPtr);

		// tex coords
		OpenGLRHI::Get().EnableVertexAttributeArray(2);
		OpenGLRHI::Get().VertexAttribPointer(2, 2, EGLDataType::Float, EGLBooleanValue::False, sizeof(Vector2D), uvDataStartPtr);

		mVAO.Unbind();
	}

	MeshResource::MeshResource()
	{
	}

	MeshResource::~MeshResource()
	{
	}

	bool MeshResource::Load(const std::string& filePath)
	{
		Assimp::Importer ModelImporter;
		const aiScene* AssimpScene = ModelImporter.ReadFile(filePath,
			aiProcess_Triangulate
			| aiProcess_GenNormals);

		bool bAssimpNotLoaded =
			!AssimpScene
			|| !AssimpScene->mRootNode
			|| AssimpScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE;

		if (bAssimpNotLoaded)
		{
			// #TODO More informative error message.
			LOG_CONSOLE_ARGS("Failed to load model from [%s].", filePath.c_str());
			return false;
		}

		ProcessNode(*AssimpScene->mRootNode, *AssimpScene);

		if (mMeshList.size() != AssimpScene->mNumMeshes)
		{
			// #TODO More informative error message.
			LOG_CONSOLE("Error reading meshes.");
			return false;
		}

		return true;
	}

	std::vector<GFXMesh*>& MeshResource::GetMeshList()
	{
		return mMeshList;
	}

	void MeshResource::ProcessNode(const aiNode& node, const aiScene& scene)
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

	void MeshResource::ProcessMesh(const aiMesh& mesh, const aiScene& scene, GFXMesh& outMesh)
	{
		bool bHasTextureCoords = mesh.mTextureCoords[0] != nullptr;
		for (U32 vertexIdx = 0; vertexIdx < mesh.mNumVertices; vertexIdx++)
		{
			const aiVector3D& assimpVert = mesh.mVertices[vertexIdx];
			const aiVector3D& assimpNormal = mesh.mNormals[vertexIdx];

			Vector3D vertPos(assimpVert.x, assimpVert.y, assimpVert.z);
			Vector3D vertNormal(assimpNormal.x, assimpNormal.y, assimpNormal.z);

			GFXVertex vertex;
			vertex.Position = vertPos;
			vertex.Normal = vertNormal;

			if (bHasTextureCoords)
			{
				const aiVector3D& assimpUV = mesh.mTextureCoords[0][vertexIdx];
				Vector2D vertUV(assimpUV.x, assimpUV.y);
				vertex.UVData = vertUV;
			}

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

		outMesh.OnLoadFinished();
	}
}
