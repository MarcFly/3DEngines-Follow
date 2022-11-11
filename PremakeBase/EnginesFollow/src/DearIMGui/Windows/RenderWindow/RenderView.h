#pragma once

#include "Globals.h"
#include "Events/Events.h"
#include "../../DearIMGuiLayer.h"
#include "Renderer/RendererEvents.h"


struct RenderView : Engine::IMGui_Item {
	std::vector<Engine::Framebuffer> framebuffers;

	int curr_selected = 0;

	int curr_id_selected = 0;
	int gl_selected_id = 0;
	
	void BuildFBStrs();
	char** strs = nullptr;
	int num_strs = 0;
	std::vector<uint32_t> ids;

	void BuildIDStrs();
	char** strs_ids = nullptr;
	int num_strs_ids = 0;

	RenderView() : IMGui_Item("Render View") {}

	void OnAttach();
	void Update();

	void OnDetach();

	DECL_DYN_EV_FUNS(UpdateFramebuffers_EventFun);
};