#pragma once

#include <Utils/PrimitiveDefs.h>

#include <Utils/Interfaces/Resource.h>


class Texture2D : public IResource
{
public:
	Texture2D();
	virtual ~Texture2D();

public:
	virtual bool Load(const FilePath& filePath) final override;
	virtual void Release() final override;

public:
	U8* GetRawData() const { return mData; }
	Vector2D GetDimensions() const;

private:
	U8* mData;

	Int32 mWidth;
	Int32 mHeight;
	Int32 mChannels;
};