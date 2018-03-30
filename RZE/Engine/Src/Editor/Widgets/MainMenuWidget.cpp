#include <StdAfx.h>
#include <Editor/Widgets/MainMenuWidget.h>

MainMenuWidget::MainMenuWidget()
{

}

MainMenuWidget::~MainMenuWidget()
{

}

void MainMenuWidget::Initialize()
{

}

void MainMenuWidget::Display()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				RZE_Engine::Get()->PostExit();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}
