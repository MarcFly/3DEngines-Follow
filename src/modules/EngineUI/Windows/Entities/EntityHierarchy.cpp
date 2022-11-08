#include "EntityHierachy.h"
#include <src/Application.h>
#include <src/modules/ECS/ModuleECS.h>

void EntityHierarchyWindow::Start() {
    inspector = (ComponentInspector*)App->engine_ui->GetItem("Inspector");
}

void EntityHierarchyWindow::UpdateRMMenu() {
    rm_menu.CheckToOpen();
    if (ImGui::BeginPopup(rm_menu.container_name))
    {
        if (ImGui::MenuItem("New Entity"))
        {
            for (int i = 0; i < selected.size(); ++i)
                App->ecs->AddEntity(selected[i]);
            if (selected.size() == 0)
                App->ecs->AddEntity(UINT64_MAX);
        }
        for (int i = 0; i < selected.size(); ++i)
            if (selected[i] == UINT64_MAX)
                continue;
        if (ImGui::MenuItem("Delete Selected"))
        {
            for (int i = 0; i < selected.size(); ++i)
                App->ecs->DeleteEntity(selected[i]);

            selected.clear();
        }
        ImGui::EndPopup();
    }
}

static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;

void EntityHierarchyWindow::UpdateEntry(Entity* curr_e) {
    bool isselected = IsSelected(curr_e->id);
    ImGuiTreeNodeFlags tmp_flags = node_flags
        | ((curr_e->children.size() == 0) * ImGuiTreeNodeFlags_Leaf)
        | (isselected * ImGuiTreeNodeFlags_Selected);

    bool open = ImGui::TreeNodeEx(&curr_e->id, tmp_flags, curr_e->name);
    if (ImGui::IsItemClicked()) {
        if (!CheckModifiers()) 
            selected.clear();
        selected.push_back(curr_e->id);
        if (inspector != nullptr) {
            if (selected.back() == App->ecs->root.id) {
                inspector->entity = &App->ecs->root;
            }
            else {
                inspector->entity = App->ecs->GetEntity(selected.back());
            }
            
        }
    }

    for (int i = 0; open && i < curr_e->children.size(); ++i)
        UpdateEntry(curr_e->children[i]);

    if (open) ImGui::TreePop();
}

void EntityHierarchyWindow::Update() {
	
	ImGui::Begin(name.c_str(), &active);
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !CheckModifiers()) {
        selected.clear();
        if (inspector != nullptr) inspector->entity = nullptr;
    }

    UpdateEntry(&App->ecs->root);

    UpdateRMMenu();

	ImGui::End();

}