#include <StdAfx.h>
#include <Game/ECS/Entity.h>

#include <Game/ECS/EntityComponent.h>

static U32 sNextEntityID = 0;

IEntity::IEntity()
{
	mEntityID = sNextEntityID++;

    // #TODO(Josh) 
    // Maybe handle this with the ComponentStorage kind of thing.
    // Will probably have to run a better implementation later.
    mComponents.resize(64, nullptr);

	SetName(std::string("Entity") + Conversions::StringFromInt(mEntityID));
}

IEntity::IEntity(const std::string& name)
{
	SetName(name);
}

IEntity::~IEntity()
{
	for (IEntityComponent* component : mComponents)
	{
		delete component;
		component = nullptr;
	}
}
