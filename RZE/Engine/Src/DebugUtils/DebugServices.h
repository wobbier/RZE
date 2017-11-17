#pragma once

#include <string>
#include <deque>

#include <Utils/Math/Vector3D.h>

class DebugServices
{
private:
	struct LogEntry
	{
		Vector3D TextColor;
		std::string Text;
	};

public:
	DebugServices();

	static void AddLog(const std::string& text, const Vector3D& color);
	static void AddData(const std::string& text, const Vector3D& color);

	static void Initialize();
	static void Display();

	static void HandleScreenResize(const Vector2D& windowSize);

private:
	static void RenderLog();
	static void RenderData();

private:
	static std::deque<LogEntry> mLogEntries;
	static std::vector<LogEntry> mDataEntries;
};