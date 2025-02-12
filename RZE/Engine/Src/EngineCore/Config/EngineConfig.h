#pragma once

#include <EngineCore/Config/Config.h>

#include <Utils/PrimitiveDefs.h>
#include <Utils/Math/Vector2D.h>

class Filepath;

class EngineSettings
{
	friend class EngineConfig;

public:
	EngineSettings();
	~EngineSettings();

public:
	bool IsVSyncEnabled() const;
	U32 GetMSAASampleCount() const;

private:
	bool bEnableVsync;
	U32 mMSAASampleCount;
};

class WindowSettings
{
	friend class EngineConfig;

public:
	WindowSettings();
	~WindowSettings();

	const Vector2D& GetDimensions() const;
	const std::string& GetTitle() const;

private:
	std::string mTitle;
	Vector2D mDimensions;
};

class EngineConfig final : public Config
{
public:
	EngineConfig();
	~EngineConfig() override;

	virtual bool Load(const Filepath& filePath) override;
	virtual void Release() override;

	const EngineSettings& GetEngineSettings() const;
	const WindowSettings& GetWindowSettings() const;

private:
	void LoadEngineSettings();
	void LoadWindowSettings();

private:
	EngineSettings mEngineSettings;
	WindowSettings mWindowSettings;
};
