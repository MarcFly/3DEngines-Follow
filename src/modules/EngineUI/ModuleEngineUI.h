#pragma once
#include <src/modules/Module.h>
#include <src/helpers/Globals.h>
#include "DearImGUI/imgui.h"
#include <libs/SDL/include/SDL.h>

class ModuleEngineUI : public Module
{
public:
	ModuleEngineUI(Application* app, bool start_enabled = true);
	~ModuleEngineUI();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	bool GetEvent(SDL_Event* e);

private:
	bool show_another_window = false;
	bool show_demo_window = true;
	bool show_simple_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};