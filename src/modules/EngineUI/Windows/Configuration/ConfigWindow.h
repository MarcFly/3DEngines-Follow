#pragma once

#include "../../ui_item.h"
#include "../../ModuleEngineUI.h"

class ConfigWindow : UI_Item {
public:
	ConfigWindow() : UI_Item("Configuration") {};
	void Start();

	void PerformanceGraphs();
	void WindowOptions();
	void HardwareInfo();
	void RenderOptions();

	void Update() {
		ImGui::Begin(name.c_str(), &active);
		
		if(ImGui::CollapsingHeader("Performance")) PerformanceGraphs();
		if(ImGui::CollapsingHeader("Windowing")) WindowOptions();
		if(ImGui::CollapsingHeader("Hardware")) HardwareInfo();

		if(ImGui::CollapsingHeader("Renderer")) RenderOptions();

		ImGui::End();
	}
};