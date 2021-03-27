#pragma once

#include <Utils/PrimitiveDefs.h>

#include <Utils/Interfaces/Resource.h>


class Texture2D : public IResource
{
	// Types
public:
	enum class ETextureType : U32
	{
		Diffuse,
		Specular,
		Normal
	};

	struct TextureProperties
	{
		ETextureType Type;

	};

public:
	static FilePath kDefaultDiffuseTexturePath;
	static FilePath kDefaultSpecularTexturePath;
	static FilePath kDefaultNormalTexturePath;

public:
	Texture2D() = delete;
	Texture2D(ETextureType textureType);
	virtual ~Texture2D();

public:
	virtual bool Load(const FilePath& filePath) final override;
	virtual void Release() final override;

public:
	U8* GetRawData() const { return mData; }
	ETextureType GetTextureType() const;
	Vector2D GetDimensions() const;

	Int32 GetGPUBufferID() const;

private:
	U8* mData;

	ETextureType mTextureType;

	Int32 mWidth;
	Int32 mHeight;
	Int32 mChannels;

	Int32 mGPUBuffer;
};