#pragma once

#include <src/helpers/Globals.h>
#include <libs/SDL/include/SDL_scancode.h>
#include <string>
#include "DearImGUI/imgui.h"
#include "ModuleEngineUI.h"

class UI_Item
{
public:
	UI_Item(const char* _name, uint32_t submenu = UINT32_MAX);
	virtual ~UI_Item() {};

	void SwitchActive() { active = !active; }
	bool IsActive() const { return active; }
	const std::string& GetName() const { return name; }

	SDL_Scancode GetShortCut() const { return shortcut; }
	void SetShortCut(SDL_Scancode _shortcut) { shortcut = _shortcut; };
	void SetInactive() { active = false; };
	
	virtual void Init() {};
	virtual void Start() {};
	virtual void PreUpdate() {};
	virtual void Update() {};
	virtual void PostUpdate() {};
	
	virtual void ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec) {};

	virtual void CleanUp() {};	

public:
	bool active = false;
	uint32_t id;

protected:
	std::string name;
	SDL_Scancode shortcut = SDL_SCANCODE_UNKNOWN;
};

class RMPopupMenu
{
	bool rm_was_clicked;
public:
	ImGuiHoveredFlags flags;
	const char* container_name;

	void CheckToOpen() {
		rm_was_clicked = (ImGui::IsWindowHovered(flags) && ImGui::IsMouseClicked(ImGuiMouseButton_Right) || rm_was_clicked);

		if (rm_was_clicked && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
			ImGui::SetNextWindowPos(ImGui::GetMousePos());
			ImGui::OpenPopup(container_name);
			rm_was_clicked = false;
		}
	}
};

