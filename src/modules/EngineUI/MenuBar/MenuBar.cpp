#include "MenuBar.h"

MenuBar menu_bar;

void MenuBar::Init()
{
}

void MenuBar::UpdateMenuItem(MenuItem& item)
{
    if (item.sub_items.size() == 0)
    {
        bool prev = *item.active_state;
        ImGui::MenuItem(item.name, "", item.active_state);
        App->engine_ui->EngineUI_RequireUpdate(!(*item.active_state != prev));
    }
    else if (ImGui::BeginMenu(item.name))
    {
        for (uint16_t i = 0; i < item.sub_items.size(); ++i)
            UpdateMenuItem(items[item.sub_items[i]]);

        ImGui::EndMenu();
    }

}

void MenuBar::Update()
{
    if (!active) return;
    ImGui::BeginMainMenuBar();

    // Should a main menu register and be a class/struct by itself or should it be standalone
    if (ImGui::BeginMenu("Menu1"))
    {
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Menu2"))
    {
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("UI WIndows"))
    {
        for (uint16_t i = 0; i < base_items.size(); ++i)
        {
            UpdateMenuItem(items[base_items[i]]);
        }

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

uint32_t MenuBar::RegisterMenuItem(bool* item_active, const char* name, uint32_t sub_item)
{
    MenuItem mi;
    mi.active_state = item_active;
    mi.name = name;
    items.push_back(mi);
    uint32_t ret = items.size() - 1;
    if (sub_item == -1) base_items.push_back(ret);
    else items[sub_item].sub_items.push_back(ret);
    return ret;
};
