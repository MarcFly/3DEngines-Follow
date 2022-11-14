#include "EnginePCH.h"
#include "DearIMGuiLayer.h"

using namespace Engine;

MenuItem::MenuItem(const char* _name, uint64_t id) : name(64, _name) {
	itemid = id;
	snprintf(imgui_id, sizeof(imgui_id), "%s##%llu\0", name.str, id);
}

MenuItem::MenuItem(const MenuItem& mi) : name(64, mi.name.str) {
	itemid = mi.itemid;
	snprintf(imgui_id, sizeof(imgui_id), "%s##%llu\0", name.str, itemid);
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

	items.emplace_back(MenuItem(name, ret));
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