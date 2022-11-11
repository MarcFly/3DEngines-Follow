#include "EnginePCH.h"
#include "EntityHierarchy.h"
#include "ECS/ECS_Events.h"
#include "ECS/ECS.h"

void EntityHierarchyWindow::RMMVirtualUpdate() {
    if (ImGui::MenuItem("New Entity"))
    {
        for (int i = 0; i < selected.size(); ++i)
            Engine::Events::SendNew(new Engine::NewEntity_Event(selected[i]));
        if (selected.size() == 0)
            Engine::Events::SendNew(new Engine::NewEntity_Event(UINT64_MAX));

    }
    if (ImGui::MenuItem("Delete Selected"))
    {
        for (int i = 0; i < selected.size(); ++i)
            Engine::Events::SendNew(new Engine::DeleteEntity_Event(selected[i]));

        selected.clear();
    }
}

static ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanFullWidth;

EVENT_TYPE_STATIC_DATA(RequestEntities_Event);
EVENT_TYPE_STATIC_DATA(ProvideEntities_Event);
EVENT_TYPE_STATIC_DATA(HierarchySelect_Event);

DEF_DYN_MEMBER_EV_FUNS(ProvideEntities_Event, EntityHierarchyWindow, UpdateEntities_EventFun) {
    read_ecs = ev->ecs;
}}

void EntityHierarchyWindow::UpdateEntry(const uint64_t eid) {
    const Engine::Entity* curr_e = read_ecs->GetEntityConst(eid);
    if (curr_e == nullptr) 
        return;

    uint32_t truncated_id = curr_e->id >> 32;

    bool isselected = IsSelected(curr_e->id);
    ImGuiTreeNodeFlags tmp_flags = node_flags
        | ((curr_e->children.size() == 0) * ImGuiTreeNodeFlags_Leaf)
        | (isselected * ImGuiTreeNodeFlags_Selected);

    bool open = ImGui::TreeNodeEx((void*)eid, tmp_flags, curr_e->name);
    
    if (ImGui::IsItemClicked()) {
        if (!CheckModifiers())
            selected.clear();
        selected.push_back(curr_e->id);
        Engine::Events::SendHotNew(new HierarchySelect_Event(curr_e->id));
    }

    for (int i = 0; open && i < curr_e->children.size(); ++i)
        UpdateEntry(curr_e->children[i]);

    if (open) ImGui::TreePop();
}

void EntityHierarchyWindow::Update() {

    ImGui::Begin(debug_name.c_str(), &active);
    if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !CheckModifiers()) {
        selected.clear();
        Engine::Events::SendHotNew(new HierarchySelect_Event(UINT64_MAX));
    }
    if (read_ecs == nullptr) {
        Engine::Events::SendHotNew(new RequestEntities_Event());
    }
    else {
        for (uint64_t eid : read_ecs->base_entities)
            UpdateEntry(eid);
    }

    RMMUpdate(debug_name.c_str());

    ImGui::End();

}