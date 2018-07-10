#pragma once

#include <Apollo/ECS/EntityComponent.h>

#include <Utils/Math/Matrix4x4.h>
#include <Utils/Math/Quaternion.h>
#include <Utils/Math/Vector3D.h>

struct TransformComponent : public Apollo::Component<TransformComponent>
{
	TransformComponent() = default;
	
	explicit TransformComponent(const Vector3D& position)
	{
		SetPosition(position);
	}

	TransformComponent(const Vector3D& position, const Quaternion& rotation)
	{
		SetPosition(position);
		SetRotation(rotation);
	}

	TransformComponent(const Vector3D& position, const Quaternion& rotation, const Vector3D& scale)
	{
		SetPosition(position);
		SetRotation(rotation);
		SetScale(scale);
	}

	//
	// Direct Setters
	//
	void SetPosition(const Vector3D& position)
	{
		Position = position;
	}

	void SetRotation(const Quaternion& rotation)
	{
		Rotation = rotation;
	}

	void SetScale(const Vector3D& scale)
	{
		Scale = scale;
	}

	//
	// Operations
	//
	void Rotate(const Quaternion& rotation)
	{
		Rotation *= rotation;
	}

	Vector3D	Position;
 	Vector3D	Scale {1, 1, 1};
	Quaternion	Rotation;
};