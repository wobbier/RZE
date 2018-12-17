#pragma once

#include <Utils/Math/Vector2D.h>
#include <Utils/PrimitiveDefs.h>

namespace Diotima
{
	class RenderTarget
	{
	public:
		RenderTarget();
		RenderTarget(U32 width, U32 height);
		~RenderTarget();

	public:
		virtual void Initialize() {}

		virtual void Bind() {}
		virtual void Unbind() {}

		inline U32 GetFrameBufferID() const { return mFrameBufferID; }
		inline U32 GetDepthBufferID() const { return mDepthBufferID; }
		inline U32 GetColorBufferID() const { return mColorBufferID; }

		inline U32 GetWidth() const { return mWidth; }
		inline U32 GetHeight() const { return mHeight; }

		// Returns true if dimensions actually changed -- to allow for current process of buffer resize
		// #NOTE(Josh::May want to address this with better architecture later?)
		bool SetDimensions(U32 width, U32 height);

	protected:
		inline U32& InternalGetFrameBufferID() { return mFrameBufferID; }
		inline U32& InternalGetDepthBufferID() { return mDepthBufferID; }
		inline U32& InternalGetColorBufferID() { return mColorBufferID; }

	private:
		U32 mFrameBufferID;
		U32 mDepthBufferID;
		U32 mColorBufferID;

		U32 mWidth;
		U32 mHeight;
	};

	class RenderTargetTexture : public RenderTarget
	{
	public:
		RenderTargetTexture() = default;
		RenderTargetTexture(U32 width, U32 height);
		virtual ~RenderTargetTexture() = default;

	public:
		inline U32 GetTextureID() { return mTextureID; }

	protected:
		inline U32& InternalGetTextureID() { return mTextureID; }

	private:
		U32 mTextureID;
	};
}
