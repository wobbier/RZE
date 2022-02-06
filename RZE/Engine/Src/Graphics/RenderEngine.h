#pragma once

#include <Graphics/Shader.h>
#include <Graphics/StaticMesh.h>

#include <Rendering/BufferHandle.h>

#include <Utils/Math/Matrix4x4.h>
#include <Utils/Math/Vector2D.h>

class MeshGeometry;

class Vector2D;

struct RenderCamera
{
	// @TODO integrate with viewport maybe
	Matrix4x4 ClipSpace;
	Vector3D Position;
};

// @TODO Move to own file
class RenderObject
{
public:
	RenderObject() = default;
	~RenderObject() = default;

	void SetStaticMesh(const StaticMesh& staticMesh) { m_staticMesh = staticMesh; }
	const StaticMesh& GetStaticMesh() { return m_staticMesh; }

	void SetTransform(const Matrix4x4& transform) { m_transform = transform; }
	const Matrix4x4& GetTransform() const { return m_transform; }

private:
	// @TODO This will be replaced after a batching system is implemented
	// Will pack all geometry into single vertex buffer and all materials into single material buffer and index away
	// or something i dunno im shit at my job. Also.. limited to just static mesh here, bad. Will end up with refactor
	// when skinned meshes are a thing
	StaticMesh m_staticMesh;
	Matrix4x4 m_transform;
};

class RenderEngine
{
public:
	RenderEngine();
	~RenderEngine();
	RenderEngine(const RenderEngine&) = delete;
	RenderEngine(const RenderEngine&&) = delete;

public:
	void operator=(const RenderEngine&) = delete;
	void operator=(const RenderEngine&&) = delete;

public:
	void Initialize(void* windowHandle);
	void Update();
	void Render();
	void Shutdown();

	std::shared_ptr<RenderObject> CreateRenderObject(const StaticMesh& staticMesh);

	void ResizeCanvas(const Vector2D& newSize);

	RenderCamera& GetCamera() { return m_camera; }

private:
	RenderCamera m_camera;

	Vector2D m_canvasSize;

	// @TODO temp until ShaderTechniques are properly implemented
	// (could be a while)
	ResourceHandle m_vertexShaderResource;
	const VertexShader* m_vertexShader = nullptr; // @TODO This is just to avoid having to GetResource() in a loop but is more a deficiency of the Resource API

	// @TODO Make not vector or something
	std::vector<std::shared_ptr<RenderObject>> m_renderObjects;
};