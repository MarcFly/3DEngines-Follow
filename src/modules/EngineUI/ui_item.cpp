#include "ui_item.h"
#include "MenuBar/MenuBar.h"

UI_Item::UI_Item(const char* _name, uint32_t submenu) : name(_name) {
	e_engine_ui.EngineUI_RegisterItem(this);
	menu_bar.RegisterMenuItem(&active, name.c_str(), submenu);
}