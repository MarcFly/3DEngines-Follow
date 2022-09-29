#include "ModuleEngineUI.h"
#include <src/Application.h>
#include "DearImGUI/backends/imgui_impl_sdl.h"
#include "DearImGUI/backends/imgui_impl_opengl3.h"
#include <gl/GL.h>
#include "ui_item.h"
#include "MenuBar/MenuBar.h"

ModuleEngineUI e_engine_ui(true);

ModuleEngineUI::ModuleEngineUI(bool start_enabled) : Module(start_enabled)
{
	
}

ModuleEngineUI::~ModuleEngineUI()
{
}

void StartImGUI(Application* App) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
    ImGui_ImplOpenGL3_Init(App->renderer3D->glsl_version);
}

#include "Windows/DemoWindow.h"

bool ModuleEngineUI::Start()
{	
    StartImGUI(App);

    EngineUI_UpdateActives();

    for (UI_Item* item : items) {
        item->Start();
    }

	return false;
}

update_status ModuleEngineUI::Update(float dt)
{
    update_status ret = UPDATE_CONTINUE;
    ImGuiIO& io = ImGui::GetIO();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
       
    // For now, just update actives each frame
    EngineUI_UpdateActives();

    // Update the main menu first
    menu_bar.Update();

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    
    for (uint32_t i = 0; i < active_items.size(); ++i)
        items[active_items[i]]->Update();

    
    if (require_update) EngineUI_UpdateActives();

	return ret;
}

update_status ModuleEngineUI::PostUpdate(float dt)
{
    // Rendering
    ImGui::Render();
    //glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
    SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    SDL_GL_MakeCurrent(backup_current_window, backup_current_context);

    return update_status::UPDATE_CONTINUE;
}

bool ModuleEngineUI::CleanUp()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

	return false;
}

bool ModuleEngineUI::GetEvent(SDL_Event* e)
{
    ImGui_ImplSDL2_ProcessEvent(e);
    return true;
}

void ModuleEngineUI::EngineUI_RegisterItem(UI_Item* item)
{
    items.push_back(item);
    item->id = items.size() - 1;
    if (item->active) active_items.push_back(item->id);
}

void ModuleEngineUI::EngineUI_UpdateActives() {
    active_items.clear();
    uint32_t s = items.size();
    for (uint32_t i = 0; i < s; ++i)
        if (items[i]->active) 
            active_items.push_back(items[i]->id);

    require_update = false;
}

