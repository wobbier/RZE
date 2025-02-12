#include <StdAfx.h>
#include <Utils/Math/Vector4D.h>

#include <Utils/Math/Vector3D.h>

#include <GLM/vec4.hpp>

Vector4D Vector4D::ZERO = Vector4D(0.0f);

Vector4D::Vector4D()
	: mVec(0.0f, 0.0f, 0.0f, 0.0f)
{
}

Vector4D::Vector4D(const float x, const float y, const float z, const float w)
	: mVec(x, y, z, w)
{
}

Vector4D::Vector4D(const int x, const int y, const int z, const int w)
	: mVec(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z), static_cast<float>(w))
{
}

Vector4D::Vector4D(const float val)
	: mVec(val, val, val, 1.0f)
{
}

float Vector4D::X() const
{
	return mVec.x;
}

float Vector4D::Y() const
{
	return mVec.y;
}

float Vector4D::Z() const
{
	return mVec.z;
}

float Vector4D::W() const
{
	return mVec.w;
}

void Vector4D::SetX(float newX)
{
	mVec.x = newX;
}

void Vector4D::SetY(float newY)
{
	mVec.y = newY;
}

void Vector4D::SetZ(float newZ)
{
	mVec.z = newZ;
}

void Vector4D::SetW(float newW)
{
	mVec.w = newW;
}

void Vector4D::Set(const float x, const float y, const float z, const float w)
{
	mVec.x = x;
	mVec.y = y;
	mVec.z = z;
	mVec.w = w;
}

const glm::vec4& Vector4D::GetInternalVec() const
{
	return mVec;
}

float Vector4D::operator[](int index) const
{
	return mVec[index];
}

void Vector4D::operator=(const Vector3D& rhs)
{
	*this = Vector4D(rhs.X(), rhs.Y(), rhs.Z(), 1.0f);
}
