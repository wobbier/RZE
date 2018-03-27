#include <StdAfx.h>
#include <EngineCore/Config/EngineConfig.h>

#include <Utils/Conversions.h>

EngineConfig::EngineConfig()
{

}

EngineConfig::~EngineConfig()
{

}

void EngineConfig::Load(const FilePath& filePath)
{
	Config::Read(filePath.GetAbsolutePath());

	LoadEngineSettings();
	LoadWindowSettings();
}

EngineSettings& EngineConfig::GetEngineSettings()
{
	return mEngineSettings;
}

WindowSettings& EngineConfig::GetWindowSettings()
{
	return mWindowSettings;
}

void EngineConfig::LoadEngineSettings()
{
	mEngineSettings.bEnableVsync = Conversions::IntFromString(mINIParser.GetValue("EngineSettings", "EnableVsync", 0)) > 0;
}

void EngineConfig::LoadWindowSettings()
{
	int width = 0;
	int height = 0;
	width = Conversions::IntFromString(mINIParser.GetValue("WindowSettings", "WindowWidth", "1280"));
	height = Conversions::IntFromString(mINIParser.GetValue("WindowSettings", "WindowHeight", "768"));

	mWindowSettings.mDimensions = Vector2D(width, height);
	mWindowSettings.mTitle = mINIParser.GetValue("WindowSettings", "WindowTitle", "RZE_Engine");
}

WindowSettings::WindowSettings()
{
}

WindowSettings::~WindowSettings()
{
}

const Vector2D& WindowSettings::GetDimensions() const
{
	return mDimensions;
}

const std::string& WindowSettings::GetTitle() const
{
	return mTitle;
}

EngineSettings::EngineSettings()
{
}

EngineSettings::~EngineSettings()
{
}

const bool EngineSettings::IsVSyncEnabled() const
{
	return bEnableVsync;
}
