#pragma once

#include <RZE.h>

#include <Windowing/Win32Window.h>

#include <memory>

class EventHandler;
class Win32Window;

namespace Rendering
{
	class RenderTargetTexture;
}

class RZE_Application
{
	// @note maybe review this connection later
	friend class RZE_Engine;
	
public:
	RZE_Application();
	virtual ~RZE_Application();

public:
	virtual void Run();
	virtual void Start();
	virtual void Update();
	virtual void ShutDown();

	virtual void CreateRenderTarget(const Vector2D& dimensions) = 0;
	virtual void OnWindowResize(const Vector2D& newSize) = 0;

	// Returning TRUE will fire callbacks registered with RZE. Returning FALSE steals input from the engine.
	virtual bool ProcessInput(const InputHandler& handler);

	virtual void ParseArguments(char** arguments, int count);

	virtual void RegisterInputEvents(InputHandler& inputHandler);


	Rendering::RenderTargetTexture* GetRTT();
	
protected:
	virtual void Initialize();

	Win32Window& InternalGetWindow();

	void SetRunning(bool bRunning);
	void ShowWindow();

	Win32Window* const GetWindow() const;

	std::unique_ptr<Rendering::RenderTargetTexture> m_renderTarget;

private:
	void SetWindow(Win32Window* const window);
	
	Win32Window* mWindow;
	bool bIsRunning;
};
