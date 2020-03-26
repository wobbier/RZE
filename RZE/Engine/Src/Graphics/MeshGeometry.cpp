#include <StdAfx.h>
#include <Graphics/MeshGeometry.h>

#include <Graphics/Material.h>

#include <Graphics/IndexBuffer.h>
#include <Graphics/VertexBuffer.h>

MeshGeometry::MeshGeometry()
{
}

MeshGeometry::~MeshGeometry()
{
}

void MeshGeometry::AllocateGPUData()
{
	std::vector<float> vertexDataBuffer;
	vertexDataBuffer.reserve(mVertices.size() * sizeof(MeshVertex));
	for (MeshVertex vertex : GetVertices())
	{
		for (int index = 0; index < 3; ++index)
		{
			vertexDataBuffer.push_back(vertex.Position[index]);
		}

// 		for (int index = 0; index < 3; ++index)
// 		{
// 			vertexDataBuffer.push_back(vertex.Normal[index]);
// 		}
// 
 		for (int index = 0; index < 2; ++index)
 		{
 			vertexDataBuffer.push_back(vertex.UVData[index]);
 		}
// 
// 		for (int index = 0; index < 3; ++index)
// 		{
// 			vertexDataBuffer.push_back(vertex.Tangent[index]);
// 		}
	}

	mVertexBuffer = std::make_shared<VertexBuffer>(vertexDataBuffer);
	mVertexBuffer->Initialize();

	mIndexBuffer = std::make_shared<IndexBuffer>(mIndices);
	mIndexBuffer->Initialize();
}

void MeshGeometry::AddVertex(const MeshVertex& vertex)
{
	mVertices.push_back(vertex);
}

void MeshGeometry::AddIndex(U32 index)
{
	mIndices.push_back(index);
}

void MeshGeometry::SetMaterial(Material* material)
{
	AssertNotNull(material);
	mMaterial = material;
}

const Material& MeshGeometry::GetMaterial() const
{
	AssertNotNull(mMaterial);
	return *mMaterial;
}

const std::vector<U32>& MeshGeometry::GetIndices() const
{
	return mIndices;
}

const std::vector<MeshVertex>& MeshGeometry::GetVertices()
{
	return mVertices;
}

U32 MeshGeometry::GetVertexBuffer() const
{
	return mVertexBuffer->GetGPUBufferIndex();
}

U32 MeshGeometry::GetIndexBuffer() const
{
	return mIndexBuffer->GetGPUBufferIndex();
}
