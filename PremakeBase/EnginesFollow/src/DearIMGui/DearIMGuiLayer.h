#pragma once

#include <imgui.h>
#include "../Globals.h"
#include "Core/Layer.h"


namespace Engine {
	struct IMGui_Item : public Layer{
		IMGui_Item(const char* name) : Layer(name) {}
		virtual ~IMGui_Item() {};

		bool active = false;
		uint32_t id;

		void SwitchActive() { active = !active; }
		bool IsActive() const { return active; }
		const std::string& GetName() const { return debug_name; }


		virtual void OnAttach() {};
		virtual void OnDetach() {};

		virtual void PreUpdate() {};
		virtual void Update() {};
		virtual void PostUpdate() {};
	};

	class RMPopupMenu {
		bool rm_was_clicked;
	public:
		ImGuiHoveredFlags rm_flags = ImGuiHoveredFlags_ChildWindows || ImGuiHoveredFlags_RootWindow;

		inline void RMMUpdate(const char* name) {
			RMMCheckToOpen(name);
			if (ImGui::BeginPopup(name))
			{
				RMMVirtualUpdate();
				ImGui::EndPopup();
			}
		}

		virtual void RMMVirtualUpdate() {}
		void RMMCheckToOpen(const char* container_name) {
			rm_was_clicked = (ImGui::IsWindowHovered(rm_flags) && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) || rm_was_clicked;

			if (rm_was_clicked && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
			{
				ImGui::SetNextWindowPos(ImGui::GetMousePos());
				ImGui::OpenPopup(container_name);
				rm_was_clicked = false;
			}
		}
	};

	struct MenuItem {
		MenuItem(const char* _name, uint64_t id);
		MenuItem(const MenuItem& mi);
		bool* active_state = nullptr;
		const char* shortcut = "";
		offload_str name;
		char imgui_id[128];
		uint64_t itemid;
		std::vector<uint32_t> sub_items;
	};

	class MenuBar : public IMGui_Item
	{
	public:
		std::vector<MenuItem> items;
		std::vector<uint32_t> base_items;
		//std::vector<MenuItem> variable_ui;
		MenuItem unordered;

	public:
		MenuBar() : IMGui_Item("Menu Bar"), unordered("Unordered", UINT32_MAX) { active = true; };

		void UpdateMenuItem(MenuItem& item);

		void Update() final;

		uint32_t RegisterMenuItem(bool* item_active, const char* name, const char* submenu = "");
	};

	struct InternalWindow;
	struct EF_API ImGuiLayer : public Layer {
		ImGuiLayer(InternalWindow* window);
		void* internalwindow = nullptr;

		void OnAttach();
		void OnDetach();

		void PreUpdate();
		void Update();
		void PostUpdate();

		template<typename I>
		void RegisterItem(const char* submenu_group) {
			items.push_back((IMGui_Item*)new I());
			IMGui_Item*& item = items.back();
			item->OnAttach();
			menubar.RegisterMenuItem(&item->active, item->debug_name.c_str(), submenu_group);
		}

		void RegisterSubmenu(const char* submenu_name, const char* group) {
			menubar.RegisterMenuItem(nullptr, submenu_name, group);
		}

		MenuBar menubar;
		std::vector<IMGui_Item*> items;
		std::vector<IMGui_Item*> active_items;

	};
};

bool CheckModifiers();