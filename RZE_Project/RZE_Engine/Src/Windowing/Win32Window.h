#pragma once

// winapi
// @improvement does it suffice to have a pch here or refactor to avoid this in a .h?
#include <Windows.h>

// std lib
#include <memory>
#include <string>

// RZE
#include "WindowMessageAdaptor.h"
#include "Utils/Math/Vector2D.h"

class EventHandler;

class Win32Window
{

private:

	struct OSWindowHandleData
	{
		HWND windowHandle;
		HDC deviceContext;
		HGLRC renderContext;
		PIXELFORMATDESCRIPTOR pixelFormatDesc;
	};

public:

	struct WindowCreationParams
	{
		std::string		windowTitle;
		int				width{ -1 };
		int				height{ -1 };
	};

	Win32Window(const WindowCreationParams& creationProtocol);

	const std::string& GetTitle() const;
	const Vector2D& GetDimensions() const;

	void CompileMessages(EventHandler& eventHandler);

	void BufferSwap() const;

	const OSWindowHandleData GetOSWindowHandleData() const;

protected:

private:

	void Create(const WindowCreationParams& creationProtocol);

	void ProcessMessage(const WindowMessageAdaptor::WindowMessageInfo& messageInfo, EventHandler& eventHandler);
		
	OSWindowHandleData mOSWindowHandleData;
	WindowCreationParams mCreationData;

	std::string mTitle;
	Vector2D mDimensions;
};
