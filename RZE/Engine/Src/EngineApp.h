#pragma once

#include <RZE.h>

class EventHandler;
class Win32Window;

class RZE_Application
{
	// @note maybe review this connection later
	friend class RZE_Engine;

public:
	RZE_Application();
	virtual ~RZE_Application();

public:
	virtual void Start();
	virtual void Update();
	virtual void ShutDown();

	virtual bool ProcessInput(const InputHandler& handler);
	virtual void RegisterInputEvents(InputHandler& inputHandler);

	virtual bool IsEditor() { return false; }

	static RZE_Engine& RZE() { return mEngine; }

protected:
	virtual void Initialize();

	void SetRunning(bool bRunning);
	void ShowWindow();

	Win32Window* const GetWindow() const;

private:
	void SetWindow(Win32Window* const window);

	Win32Window* mWindow;
	bool bIsRunning;

	static RZE_Engine mEngine;
};
