#pragma once

#include <vector>

#include <Diotima/Driver/OpenGL.h>

#include <Utils/Interfaces/Resource.h>
#include <Utils/Math/Vector2D.h>
#include <Utils/Math/Vector3D.h>

struct aiMesh;
struct aiNode;
struct aiScene;

namespace Diotima
{
	struct GFXVertex
	{
		Vector3D Position;
		Vector3D Normal;
		Vector2D UVData;
	};

	class GFXMesh
	{
		friend class MeshResource;

	public:
		GFXMesh();
		~GFXMesh();

		OpenGLVAO& GetVAO();
		OpenGLEBO& GetEBO();
		OpenGLVBO& GetVertexVBO();

		const std::vector<GFXVertex>& GetVertexList() const;
		const std::vector<U32>& GetIndices() const;

		void AddVertex(const GFXVertex& vertex);
		void AddIndex(U32 index);

		void OnLoadFinished();

	private:
		OpenGLVAO mVAO;
		OpenGLVBO mVertexVBO;
		OpenGLVBO mNormalVBO;
		OpenGLEBO mEBO;

		std::vector<GFXVertex> mVertices;
		std::vector<Vector3D> mPositions;
		std::vector<Vector3D> mNormals;
		std::vector<Vector2D> mUVCoords;
		std::vector<U32> mIndices;
	};
}
