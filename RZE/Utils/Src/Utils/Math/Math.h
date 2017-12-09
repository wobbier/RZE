#pragma once

#include <Utils/Math/Quaternion.h>
#include <Utils/Math/Vector3D.h>

namespace MathUtils
{
	const float Pi = 3.14159265359f;

	inline float ToRadians(float num)
	{
		return num * (Pi / 180.0f);
	}
}

namespace VectorUtils
{
	Vector3D	Lerp(const Vector3D& from, const Vector3D& to, const float factor);
	float		DistanceSq(const Vector3D& from, const Vector3D& to);
}