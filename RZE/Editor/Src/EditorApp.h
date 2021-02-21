#pragma once

#include <EngineApp.h>

#include <UI/Panels/LogPanel.h>
#include <UI/Panels/ScenePanel.h>
#include <UI/Panels/SceneViewPanel.h>

struct ImFont;

namespace Editor
{
	class EditorApp : public RZE_Application
	{
	private:
		struct PanelStates
		{
			bool bDemoPanelEnabled{ false };
			bool bScenePanelEnabled{ false };
		};

	public:
		EditorApp();
		virtual ~EditorApp();

		virtual void Initialize() override;
		virtual void Start() override;
		virtual void Update() override;
		virtual void ShutDown() override;

		virtual void ParseArguments(const char* arguments, int count);
		virtual void RegisterInputEvents(InputHandler& inputHandler) override;
		virtual bool ProcessInput(const InputHandler& handler) override;

	public:
		void SetFont(const char* fontName);

		void Log(const std::string& msg);

	private:
		void DisplayMenuBar();
		void HandleGeneralContextMenu();
		void ResolvePanelState();

		void LoadFonts();
		void StyleSetup();

		void RunAssetCpy();

	private:
		PanelStates mPanelStates;

		LogPanel mLogPanel;
		ScenePanel mScenePanel;
		SceneViewPanel mSceneViewPanel;

		std::unordered_map<std::string, ImFont*> mFontMapping;

		const float kMenuBarHeight = 24.f;
	};
}