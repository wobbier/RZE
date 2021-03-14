#pragma once

#include <functional>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <vector>

#include <Diotima/RenderCommands.h>

#include <Utils/Math/Matrix4x4.h>
#include <Utils/Math/Vector2D.h>
#include <Utils/PrimitiveDefs.h>

struct ID3D11InputLayout;

namespace Diotima
{
	class DX11GFXDevice;

	class GFXPassGraph;
	class RenderTargetTexture;

	// #TODO
	// This is just here as a reminder to figure this out.
	// This is how the driver state needs to be set to draw an item.
	// Might be a good idea to investigate using the opt-in/out defaults system.
	// If something hasn't been overridden or excluded, it will use a default state.
	// Then there is no state leakage because everything just sets what it needs.
	struct GraphicsState
	{

	};

	// Essentially a direct copy into a constant buffer. All this data is consumed by the GPU
	// for lighting etc
	struct MaterialData
	{
		Int32 TextureBuffer = -1;
		float Shininess;
	};

	// Bundle of data that is necessary for each draw call.
	// #TODO
	// These will need to be wrapped or otherwise sorted properly by some higher-level design.
	// This is simply the data for any item with geometry. When and how this data is used is determined
	// elsewhere. The Int32 handles should be designed such that -1 means no data. 
	// For now, each item will need a Vertex and Index buffer, but should be easily extensible
	// to support other types of data construction or use.
	struct RenderObject
	{
		Int32 VertexBuffer = -1;
		Int32 IndexBuffer = -1;
		Int32 ConstantBuffer = -1;
		MaterialData Material;
		Matrix4x4 Transform;
	};

	struct RenderObjectHandle
	{
		U32 Value;
	};

	class Renderer
	{
		// Constructors
	public:
		Renderer();
		~Renderer();

	public:
		void Initialize();
		void Update();
		void Render();
		void ShutDown();

	public:
		
		// #TODO(Josh::Really really don't like this, fix later)
		void SetWindow(void* handle) { mWindowHandle = handle; }

		void EnableVsync(bool bEnable);

		const Vector2D& GetCanvasSize();
		void ResizeCanvas(const Vector2D& newSize);

		void SetViewportSize(const Vector2D& newSize);
		const Vector2D& GetViewportSize();

		void ProcessCommands();

		// #TODO
		// private later
		Int32 CreateVertexBuffer(void* data, size_t size, U32 count);
		Int32 CreateIndexBuffer(void* data, size_t size, U32 count);
		Int32 CreateTextureBuffer2D(void* data, U32 width, U32 height);

		RenderObjectHandle CreateRenderObject();

	private:
		// #TODO
		// Eventually needs to sort on key.
		std::vector<RenderObject> mRenderObjects;
		std::queue<U32> mFreeRenderObjectIndices;

		Vector2D mCanvasSize;
		RenderTargetTexture* mRenderTarget;

	private:
		void* mWindowHandle;
		std::unique_ptr<DX11GFXDevice> mDevice;
		Vector2D mViewportDimensions;
	};
}
