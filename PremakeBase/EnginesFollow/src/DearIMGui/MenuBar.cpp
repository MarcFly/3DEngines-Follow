#include "EnginePCH.h"
#include "DearIMGuiLayer.h"

using namespace Engine;

MenuItem::MenuItem(const char* _name, uint32_t id) : name(64, _name) {
	sprintf(imgui_id, "%s##%u\0", name.str, id);
}

void MenuBar::UpdateMenuItem(MenuItem& item) {
	if (item.sub_items.size() == 0 && item.active_state != nullptr) {
		ImGui::MenuItem(item.imgui_id, item.shortcut, item.active_state);
	}
	else if(ImGui::BeginMenu(item.imgui_id)) {
		for (uint32_t i : item.sub_items)
			UpdateMenuItem(items[i]);

		ImGui::EndMenu();		
	}
}

void MenuBar::Update() {
	ImGui::BeginMainMenuBar();
	for (uint32_t i : base_items) {
		UpdateMenuItem(items[i]);
	}
	
	UpdateMenuItem(unordered);
	ImGui::EndMainMenuBar();
}

uint32_t MenuBar::RegisterMenuItem(bool* item_active, const char* name, const char* group) {
	uint32_t ret = items.size();

	items.push_back(MenuItem(name, ret));
	MenuItem& mi = items.back();
	mi.active_state = item_active;
	if (!strcmp(group, "")) {
		if (item_active != nullptr)
			unordered.sub_items.push_back(ret);
		else
			base_items.push_back(ret);
	}
	else {
		bool pushed = false;
		for (MenuItem& i : items)
			if (!strcmp(group, i.name.str)) {
				i.sub_items.push_back(ret);
				pushed = true;
				break;
			}

		if (!pushed) {
			RegisterMenuItem(nullptr, group);

			for (MenuItem& i : items)
				if (!strcmp(group, i.name.str)) {
					i.sub_items.push_back(ret);
					break;
				}
		}
	}
	
	return ret;
}