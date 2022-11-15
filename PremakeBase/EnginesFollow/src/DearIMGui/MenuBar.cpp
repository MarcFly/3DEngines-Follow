#include "EnginePCH.h"
#include "DearIMGuiLayer.h"

using namespace Engine;

MenuItem::MenuItem(const char* _name, uint64_t id) : name(_name), imgui_id(name.str.get()) {
	itemid = id;
	snprintf(imgui_id.str.get(), sizeof(imgui_id), "%s##%llu\0", name.str.get(), id);
}

MenuItem::MenuItem(const MenuItem& mi) : name(mi.name), itemid(mi.itemid), sub_items(mi.sub_items),
imgui_id(mi.imgui_id), shortcut(mi.shortcut), active_state(mi.active_state) {}


void MenuBar::UpdateMenuItem(MenuItem& item) {
	if (item.sub_items.size() == 0 && item.active_state != nullptr) {
		ImGui::MenuItem(item.imgui_id.str.get(), item.shortcut, item.active_state);
	}
	else if(ImGui::BeginMenu(item.imgui_id.str.get())) {
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
			if (!strcmp(group, i.name.str.get())) {
				i.sub_items.push_back(ret);
				pushed = true;
				break;
			}

		if (!pushed) {
			RegisterMenuItem(nullptr, group);
			for(int i = 0; i < items.size(); ++i)
				if (!strcmp(group, items[i].name.str.get())) {
					MenuItem& migroup = items[i];
					migroup.sub_items.push_back(ret);
					break;
				}
		}
	}
	
	return ret;
}