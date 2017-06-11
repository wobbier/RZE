#include "StdAfx.h"

#include "Game/Components/TransformComponent.h"

TransformComponent::TransformComponent()
	: mPosition(0.0f, 0.0f, 0.0f)
	, mScale(1.0f, 1.0f, 1.0f)
	, mRotation(Vector3D())
{
}

Vector3D& TransformComponent::GetPosition()
{
	return mPosition;
}

Vector3D& TransformComponent::GetScale()
{
	return mScale;
}

Quaternion& TransformComponent::GetRotation()
{
	return mRotation;
}

void TransformComponent::SetPosition(const Vector3D& newPosition)
{
	mPosition = newPosition;
}

void TransformComponent::SetScale(const Vector3D& newScale)
{
	mScale = newScale;
}

void TransformComponent::SetRotation(const Quaternion& newRotation)
{
	mRotation = newRotation;
}
