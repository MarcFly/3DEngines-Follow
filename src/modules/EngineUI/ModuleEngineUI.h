#pragma once
#include <src/modules/Module.h>
#include <src/helpers/Globals.h>
#include "DearImGUI/imgui.h"
#include <libs/SDL/include/SDL.h>
#include <vector>

class UI_Item;
class MenuBar;

class ModuleEngineUI : public Module
{
public:
	ModuleEngineUI(bool start_updated);
	~ModuleEngineUI();

	bool Start();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec);

	bool GetEvent(SDL_Event* e);
	
	void EngineUI_RegisterItem(UI_Item* item);
	void EngineUI_RequireUpdate(bool window_state)
	{
		require_update = require_update || !window_state;
	}

private:
	MenuBar* menu_bar;
	std::vector<UI_Item*> items;
	std::vector<uint32_t> active_items;
	bool require_update = false;

private:
	void EngineUI_UpdateActives();
};

bool CheckModifiers();

extern ModuleEngineUI e_engine_ui;


