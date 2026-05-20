#include "pch.h"

#include "imgui-master/imgui.h"
#include "UI/UIManager.h"
#include "Entities/Factory/EntityBuilder.h"

namespace DebugMenu
{
	void DoUIWindow()
	{
		UIManager& ui_manager = UIManager::Get();

		std::vector<const char*> screens;
		for (auto iter = ui_manager.screenMetaData.begin(); iter != ui_manager.screenMetaData.end(); iter++)
		{
			screens.push_back(iter->first.c_str());
		}

		int index = -1;
		if (ImGui::Combo("Open Screen", &index, screens.data(), (int)screens.size()))
		{
			ui_manager.OpenScreen(screens[index]);
		}
	}
}