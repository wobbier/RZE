#include <StdAfx.h>
#include <Utils/Math/Quaternion.h>

#include <Utils/Math/Vector3D.h>

Quaternion::Quaternion()
	: mQuat(Vector3D().GetInternalVec())
{
}

Quaternion::Quaternion(const Vector3D& angles)
	: mQuat(angles.GetInternalVec())
{
}

float Quaternion::ToAngle() const
{
	return glm::angle(mQuat);
}

Vector3D Quaternion::ToAxis() const
{
	glm::vec3 axis = glm::axis(mQuat);
	return Vector3D(axis.x, axis.y, axis.z);
}

const glm::quat& Quaternion::GetInternalQuat()
{
	return mQuat;
}
