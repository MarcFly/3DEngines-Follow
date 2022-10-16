#include "EntityHierachy.h"
#include <src/Application.h>
#include <src/modules/ECS/ModuleECS.h>

void EntityHierarchyWindow::UpdateRMMenu() {
    rm_menu.CheckToOpen();
    if (ImGui::BeginPopup(rm_menu.container_name))
    {
        if (ImGui::MenuItem("New Entity"))
        {
            for (int i = 0; i < selected.size(); ++i)
                EV_SEND_UINT64(ECS_REQUEST_NEW_ENTITY, selected[i]);
            if(selected.size() == 0)
                EV_SEND_UINT64(ECS_REQUEST_NEW_ENTITY, UINT64_MAX);

        }
        if (ImGui::MenuItem("Delete Selected"))
        {
            for (int i = 0; i < selected.size(); ++i)
                EV_SEND_UINT64(ECS_REQUEST_DELETE_ENTITY, selected[i]);

            selected.clear();
        }
        ImGui::EndPopup();
    }
}

static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;

void EntityHierarchyWindow::UpdateEntry(const uint64_t eid) {
    Entity* curr_e = App->ecs->GetEntity(eid);
    bool isselected = IsSelected(curr_e->id);
    ImGuiTreeNodeFlags tmp_flags = node_flags
        | ((curr_e->children.size() == 0) * ImGuiTreeNodeFlags_Leaf)
        | (isselected * ImGuiTreeNodeFlags_Selected);

    bool open = ImGui::TreeNodeEx(&curr_e->id, tmp_flags, curr_e->name.c_str());
    if (ImGui::IsItemClicked()) {
        if (!CheckModifiers()) 
            selected.clear();
        selected.push_back(curr_e->id);
        // TODO: Send to the inspector latest added
    }

    for (int i = 0; open && i < curr_e->children.size(); ++i)
        UpdateEntry(curr_e->children[i]);

    if (open) ImGui::TreePop();
}

void EntityHierarchyWindow::Update() {
	
	ImGui::Begin(name.c_str(), &active);
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !CheckModifiers())
        selected.clear();

    for (auto eid : App->ecs->base_entities)
        UpdateEntry(eid);

    UpdateRMMenu();

	ImGui::End();

}