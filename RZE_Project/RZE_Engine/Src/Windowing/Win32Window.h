#pragma once

// winapi
// @improvement does it suffice to have a pch here or refactor to avoid this in a .h?
#include <Windows.h>

// std lib
#include <memory>
#include <string>

// RZE
#include <Windowing/WindowMessageAdaptor.h>
#include <Utils/Math/Vector2D.h>

class EventHandler;

class Win32Window
{

private:

	typedef struct OSWindowHandleData
	{
		HWND windowHandle;
		HDC deviceContext;
		HGLRC renderContext;
		PIXELFORMATDESCRIPTOR pixelFormatDesc;
	} OSWindowHandleData;

public:

	struct WindowCreationParams
	{
		std::string		windowTitle;
		int				width{ -1 };
		int				height{ -1 };
	};

	Win32Window(const WindowCreationParams& creationProtocol);

	void SetCursorEnabled(bool enabled);
	void ResetCursorToCenter();

	const std::string& GetTitle() const;
	const Vector2D& GetDimensions() const;

	void CompileMessages(EventHandler& eventHandler);

	void Show();

	void BufferSwap() const;

	const OSWindowHandleData& GetOSWindowHandleData() const;

protected:

private:

	void Create(const WindowCreationParams& creationProtocol);

	void ProcessWinProcMessage(const WindowMessageAdaptor::WindowMessageInfo& messageInfo, EventHandler& eventHandler);

	void InternalSetWindowPosition(const Vector2D& pos);

	void InternalSetWindowSize(const Vector2D& size);

	OSWindowHandleData mOSWindowHandleData;
	WindowCreationParams mCreationData;

	std::string mTitle;
	Vector2D mDimensions;
	Vector2D mPos;

	bool bCursorEnabled;
};
