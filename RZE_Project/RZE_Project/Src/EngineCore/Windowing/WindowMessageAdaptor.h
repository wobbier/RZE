#pragma once

#include <queue>

class WindowMessageAdaptor
{
public:

	struct EMessageType
	{
		enum Value
		{
			Window_KeyUp,
			Window_KeyDown,
			Window_Move
		};
	};

	struct WindowMessageInfo
	{
		EMessageType::Value		mMessageType;
		UInt64					wParam;
		Int64					lParam;
	};

	WindowMessageAdaptor();

	WindowMessageInfo GetNextMessage();
	void PushMessage(const EMessageType::Value messageType, UInt64 wParam, Int64 lParam);

	bool HasMessage();

private:

	std::queue<WindowMessageInfo> mMessageQueue;
};
