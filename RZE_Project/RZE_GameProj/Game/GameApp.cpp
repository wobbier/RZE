#include "GameApp.h"

#include "RZE.h"

#include "Debug/Debug.h"

GameApp::GameApp()
	: RZE_Game()
{
}

GameApp::~GameApp()
{
}

void GameApp::RegisterEvents(EventHandler& eventHandler)
{
	//Functor<void, const Event&> keyEvent([this](const Event& evt)
	//{
	//	if (evt.mInfo.mEventType == EEventType::Key)
	//	{
	//		LOG_CONSOLE_ARGS("KeyPress: %c\n", static_cast<char>(evt.mKeyEvent.mKey));
	//	}
	//});

	//eventHandler.RegisterForEvent(EEventType::Key, keyEvent);
}

void GameApp::Update()
{
	RZE_Game::Update();
}
