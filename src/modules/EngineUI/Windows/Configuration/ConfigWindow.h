#pragma once

#include "../../ui_item.h"
#include "../../ModuleEngineUI.h"

class ConfigWindow : UI_Item {
public:
	ConfigWindow() : UI_Item("Configuration") {};

	void PerformanceGraphs();
	void WindowOptions();
	void HardwareInfo();

	void Update() {
		ImGui::Begin(name.c_str(), &active);
		
		PerformanceGraphs();
		WindowOptions();
		HardwareInfo();

		ImGui::End();
	}
};