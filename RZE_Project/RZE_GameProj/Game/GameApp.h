#pragma once

#include <vector>

#include <Application.h>

class GameEntity;

class GameApp : public RZE_Game
{
public:

    GameApp();
    virtual ~GameApp();

    virtual void RegisterEvents(EventHandler& eventHandler) override;

    virtual void Start() override;
    virtual void Update() override;

private:
    GameEntity* mLightEntity;
    std::vector<GameEntity*> mEntities;
};