#include <StdAfx.h>
#include <Game/World/GameObjectComponents/TransformComponent.h>

#include <Utils/Utils.h>

TransformComponent::TransformComponent(const Vector3D& position)
{
	SetPosition(position);
}

TransformComponent::TransformComponent(const Vector3D& position, const Vector3D& rotation)
{
	SetPosition(position);
	SetRotation(rotation);
}

TransformComponent::TransformComponent(const Vector3D& position, const Vector3D& rotation, const Vector3D& scale)
{
	SetPosition(position);
	SetRotation(rotation);
	SetScale(scale);
}

void TransformComponent::Update()
{
	if (m_isDirty)
	{
		if (!IsRoot())
		{
			m_localTransform = Matrix4x4::CreateInPlace(m_position, m_scale, m_rotation);
			m_worldTransform = m_parent->GetWorldMatrix() * m_localTransform;
		}
		else
		{
			m_worldTransform = Matrix4x4::CreateInPlace(m_position, m_scale, m_rotation);
		}

		m_isDirty = false;
	}
}

Vector3D& TransformComponent::GetPosition()
{
	return m_position;
}

Vector3D& TransformComponent::GetRotation()
{
	return m_rotation;
}

Vector3D& TransformComponent::GetScale()
{
	return m_scale;
}

void TransformComponent::OnEditorInspect()
{
	float* positionValues = const_cast<float*>(&m_position.GetInternalVec().x);
	float* rotationValues = const_cast<float*>(&m_rotation.GetInternalVec().x);
	float* scaleValues = const_cast<float*>(&m_scale.GetInternalVec().x);

	// #TODO(Modify DragFloat3 to only register on mouse movement to avoid falloff when dragging)

	bool valueChanged = false;

	ImGui::Text("Position");
	valueChanged |= ImGui::InputFloat3("##transfcomp_position", positionValues, nullptr, ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::Text("Rotation");
	valueChanged |= ImGui::InputFloat3("##transfcomp_rotation", rotationValues, nullptr, ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::Text("Scale");
	valueChanged |= ImGui::InputFloat3("##transfcomp_scale", scaleValues, nullptr, ImGuiInputTextFlags_EnterReturnsTrue);

	if (valueChanged)
	{
		MarkDirty();
	}
}

void TransformComponent::SetPosition(const Vector3D& position)
{
	if (m_position != position)
	{
		m_position = position;
		MarkDirty();
	}
}

void TransformComponent::SetRotation(const Vector3D& rotation)
{
	if (m_rotation != rotation)
	{
		m_rotation = rotation;
		MarkDirty();
	}
}

void TransformComponent::SetScale(const Vector3D& scale)
{
	if (m_scale != scale)
	{
		m_scale = scale;
		MarkDirty();
	}
}

const Matrix4x4& TransformComponent::GetWorldMatrix() const
{
	return m_worldTransform;
}

void TransformComponent::RotateBy(const Vector3D& rotation)
{
	m_rotation += rotation;
	MarkDirty();
}

void TransformComponent::AttachTo(TransformComponent* parent)
{
	AssertIsNull(m_parent);
	parent->AddChild(this);
	m_parent = parent;
	
	MarkDirty();
}

void TransformComponent::DetachFromParent()
{
	AssertNotNull(m_parent);
	m_parent->RemoveChild(this);
	m_parent = nullptr;
}

void TransformComponent::AddChild(TransformComponent* child)
{
	AssertNotNull(child);
#if _DEBUG
	auto existingChild = std::find_if(m_children.begin(), m_children.end(), [child](TransformComponent* component)
		{
			return component == child;
		});
	AssertExpr(existingChild == m_children.end());
#endif

	m_children.push_back(child);
}

void TransformComponent::RemoveChild(TransformComponent* child)
{
	auto foundChild = std::find_if(m_children.begin(), m_children.end(), [child](TransformComponent* component)
		{
			return component == child;
		});

	if (foundChild != m_children.end())
	{
		ContainerUtils::VectorEraseBack(m_children, foundChild);
	}
}

bool TransformComponent::IsRoot()
{
	return m_parent == nullptr;
}

void TransformComponent::MarkDirty()
{
	m_isDirty = true;

	for (auto& child : m_children)
	{
		child->MarkDirty();
	}
}

const Vector3D& TransformComponent::GetPosition() const
{
	return m_position;
}

const Vector3D& TransformComponent::GetRotation() const
{
	return m_rotation;
}

const Vector3D& TransformComponent::GetScale() const
{
	return m_scale;
}

void TransformComponent::Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.String("TransformComponent");
	writer.StartObject();
	{
		{
			writer.Key("Position");
			writer.StartArray();
			for (int i = 0; i < 3; ++i)
			{
				writer.Double(m_position[i]);
			}
			writer.EndArray();
		}
		{
			writer.Key("Scale");
			writer.StartArray();
			for (int i = 0; i < 3; ++i)
			{
				writer.Double(m_scale[i]);
			}
			writer.EndArray();
		}
		{
			writer.Key("Rotation");
			writer.StartArray();
			for (int i = 0; i < 3; ++i)
			{
				writer.Double(m_rotation[i]);
			}
			writer.EndArray();
		}
	}
	writer.EndObject();
}

void TransformComponent::Deserialize(const rapidjson::Value& data)
{
	m_position = Vector3D(data["Position"][0].GetFloat(), data["Position"][1].GetFloat(), data["Position"][2].GetFloat());
	m_scale = Vector3D(data["Scale"][0].GetFloat(), data["Scale"][1].GetFloat(), data["Scale"][2].GetFloat());
	m_rotation = Vector3D(data["Rotation"][0].GetFloat(), data["Rotation"][1].GetFloat(), data["Rotation"][2].GetFloat());
}
