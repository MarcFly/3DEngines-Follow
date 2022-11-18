#include "EnginePCH.h"

#include "DearIMGuiLayer.h"
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include "Input/InputEnums.h"
#include "Core/Window.h"

#include "Windows/EntityHierarchy/EntityHierarchy.h"
#include "Windows/RenderWindow/RenderView.h"
#include "Windows/EntityInspector/EntityInspector.h"

using namespace Engine;

ImGuiLayer::ImGuiLayer(InternalWindow* window) : Layer("DearImGui") {
	internalwindow = window->internal_window;
}

void ImGuiLayer::OnAttach() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL((GLFWwindow*)internalwindow, true);
	ImGui_ImplOpenGL3_Init("#version 150");
	
	// Register some default itemss
	// Should be application decided, I provide the templates, user adds them
	RegisterItem<EntityHierarchyWindow>("Entity Management");
	RegisterItem<RenderView>("Render");
	RegisterItem<EntityInspector>("Entity Management");
}

void ImGuiLayer::OnDetach() {
	active_items.clear();
	for (IMGui_Item* item : items) {
		item->OnDetach();
		delete item;
	}
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiLayer::PreUpdate() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	
	// TODO: Update menu items and events...
	menubar.Update();	
	
	active_items.clear();
	uint32_t s = items.size();
	for (uint32_t i = 0; i < s; ++i)
		if (items[i]->active)
			active_items.push_back(items[i]);

	//require_update = false;
}

void ImGuiLayer::Update() {
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
	
	// TODO: Update items and events...
	for (uint32_t i = 0; i < active_items.size(); ++i)
		active_items[i]->Update();
}

void ImGuiLayer::PostUpdate() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();

	// TODO: udpate events
}

bool CheckModifiers() {
	{
		return ImGui::IsKeyDown((ImGuiKey)Engine::INPUT::KB::LEFT_CTRL)
			|| ImGui::IsKeyDown((ImGuiKey)Engine::INPUT::KB::LEFT_SHIFT)
			|| ImGui::IsKeyDown((ImGuiKey)Engine::INPUT::KB::RIGHT_CTRL)
			|| ImGui::IsKeyDown((ImGuiKey)Engine::INPUT::KB::RIGHT_SHIFT);
	}
}