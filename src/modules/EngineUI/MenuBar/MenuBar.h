#pragma once
#include "../ui_item.h"
#include "../ModuleEngineUI.h"
#include <src/Application.h>

struct SubMenu
{
    const char* name;
    std::vector<uint32_t> items;
    std::vector<uint32_t> sub_menus;
};

struct MenuItem
{
    bool* active_state;
    const char* name;
    std::vector<uint32_t> sub_items;
};



class MenuBar : public UI_Item
{
public:
    std::vector<MenuItem> items;
    std::vector<uint32_t> base_items;
    std::vector<MenuItem> variable_ui;

    bool registerable = false;
    MenuBar() : UI_Item("Menu Bar") { active = true; };

    void Init() final;

    void UpdateMenuItem(MenuItem& item);
    

    void Update() final;

    void CleanUp() final {}

    uint32_t RegisterMenuItem(bool* item_active, const char* name, const char* submenu = "");
};