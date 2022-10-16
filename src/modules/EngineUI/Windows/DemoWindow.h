#pragma once

#include "../ui_item.h"
#include "../MenuBar/MenuBar.h"

class DemoWindow : UI_Item {
public:
	DemoWindow() : UI_Item("DemoWindow") {}

	void Update() {
		ImGui::ShowDemoWindow(&active);
	}
};