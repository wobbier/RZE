#include <StdAfx.h>
#include <Utils/Math/Vector3d.h>

Vector3D::Vector3D()
	: mVec(0.0f, 0.0f, 0.0f)
{
}

Vector3D::Vector3D(const float x, const float y, const float z)
	: mVec(x, y, z)
{
}

float Vector3D::X() const
{
	return mVec.x;
}

float Vector3D::Y() const
{
	return mVec.y;
}

float Vector3D::Z() const
{
	return mVec.z;
}

const glm::vec3 Vector3D::GetInternalVec() const
{
	return mVec;
}
