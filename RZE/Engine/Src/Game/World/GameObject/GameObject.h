#pragma once

#include <Game/World/GameObject/GameObjectDefinitions.h>
#include <Game/World/GameObject/GameObjectComponent.h>

#include <Utils/DebugUtils/Debug.h>

typedef U32 GameObjectID;

class GameObjectComponentBase;

class GameObject
{
public:
	GameObject();
	GameObject(const std::string& name);
	~GameObject();

public:
	template <typename TComponentType, typename... Args>
	TComponentType* AddComponent(Args... args);
	
	template <typename TComponentType>
	TComponentType* GetComponent();

	template <typename TComponentType>
	void RemoveComponent();

	void OnAddToScene();

	// #TODO Unsure if it's desired to update via the GameObject API but until convinced otherwise this is it
	void Update();

	GameObjectID GetID() const { return m_id; }

	void SetName(const std::string& name) { m_name = name; }
	const std::string& GetName() const { return m_name; };

public:
	void Save(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer) const;
	void Load(rapidjson::Value& data);

private:
	// Should only be used by load code
	GameObjectComponentBase* AddComponentByID(GameObjectComponentID id);

private:
	GameObjectID m_id;
	std::string m_name;

	std::vector<GameObjectComponentBase*> m_components;
};

template <typename TComponentType>
TComponentType* GameObject::GetComponent()
{
	// #TODO Slow function

	auto it = std::find_if(m_components.begin(), m_components.end(),
		[](const GameObjectComponentBase* component)
		{
			return TComponentType::GetID() == component->m_id;
		});

	if (it != m_components.end())
	{
		return static_cast<TComponentType*>(*it);
	}

	return nullptr;
}

template <typename TComponentType>
void GameObject::RemoveComponent()
{
	// #TODO Slow function

	auto it = std::find_if(m_components.begin(), m_components.end(),
		[](const GameObjectComponentBase* component)
		{
			return TComponentType::GetID() == component->m_id;
		});

	if (it != m_components.end())
	{
		const TComponentType* component = it;
		m_components.erase(it);

		// #TODO uninitialize here
		delete component;
	}
}

template <typename TComponentType, typename ... Args>
TComponentType* GameObject::AddComponent(Args... args)
{
	// #TODO Slow function

	auto it = std::find_if(m_components.begin(), m_components.end(),
		[](const GameObjectComponentBase* component)
		{
			return TComponentType::GetID() == component->m_id;
		});

	if (it == m_components.end())
	{
		TComponentType* component = new TComponentType(std::forward<Args>(args));
		component->SetOwner(this);
		m_components.push_back(component);

		// #TODO Initialize here
		return component;
	}

	LOG_CONSOLE_ARGS("Failed to add component %s as a component of that type already exists", TComponentType::GetComponentName());
	return nullptr;
}
