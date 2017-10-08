#include <StdAfx.h>

#include <imGUI/imgui.h>

#include <Editor/Systems/EditorUISystem.h>

#include <Editor/Components/Windows/EditorLogWindow.h>

EditorUISystem::EditorUISystem(IEntityAdmin* admin)
	: IEntitySystem(admin)
{

}

EditorUISystem::~EditorUISystem()
{

}

void EditorUISystem::Init()
{
	mRelevantComponents.AddFilterType<EditorLogWindow>();
}

void EditorUISystem::Update()
{
	std::vector<IEntity*> entities;
	mRelevantComponents.FilterEachOf(mAdmin->GetEntities(), entities);

	for (auto& entity : entities)
	{
		EditorLogWindow* logWindow = entity->GetComponent<EditorLogWindow>();
		AssertNotNull(logWindow);

		ImGui::SetNextWindowPos(ImVec2(logWindow->GetPosition().X(), logWindow->GetPosition().Y()));
		ImGui::SetNextWindowSize(ImVec2(logWindow->GetDimensions().X(), logWindow->GetDimensions().Y()));
		ImGui::Begin(logWindow->GetName().c_str());
		ImGui::End();
	}
}

void EditorUISystem::ShutDown()
{

}
