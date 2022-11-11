#include "EnginePCH.h"
#include "RenderView.h"

void RenderView::BuildFBStrs() {
	if (num_strs != 0) {
		for (int i = 0; i < num_strs; ++i)
			delete strs[i];
		delete[] strs;
		num_strs = 0;
	}

	num_strs = framebuffers.size();
	strs = new char* [num_strs];
	for (int i = 0; i < num_strs; ++i) {
		strs[i] = new char[32];
		snprintf(strs[i], 32, "Framebuffer %d", i);
	}

	BuildIDStrs();
}

DEF_DYN_MEMBER_EV_FUNS(Engine::ProvideFramebuffers_Event, RenderView, UpdateFramebuffers_EventFun) {
	framebuffers = ev->framebuffers;
	if (curr_selected > framebuffers.size()) curr_selected = 0;

	BuildFBStrs();
}}

void RenderView::BuildIDStrs() {
	Engine::Framebuffer& fb = framebuffers[curr_selected];

	ids.clear();
	if (num_strs_ids != 0) {
		for (int i = 0; i < num_strs_ids; ++i)
			delete strs_ids[i];
		delete[] strs_ids;
		num_strs_ids = 0;
	}

	num_strs_ids = fb.attachments.size();
	strs_ids = new char* [num_strs_ids];
	for (int j = 0; j < fb.attachments.size(); ++j) {
		strs_ids[j] = new char[32];
		snprintf(strs_ids[j], 32, "Attachment %d", j);
		ids.push_back(fb.attachments[j].tex_id);
	}

	if (gl_selected_id > num_strs_ids) gl_selected_id = 0;

	
}

void RenderView::OnAttach() {
	Engine::Events::SubscribeDyn<Engine::ProvideFramebuffers_Event>(UpdateFramebuffers_EventFunRedirect, this);
}

void RenderView::OnDetach() {
	for (int i = 0; i < num_strs; ++i)
		delete strs[i];
	delete[] strs;
}

void RenderView::Update() {
	float pad = ImGui::GetStyle().WindowPadding.x;
	ImGui::Begin(debug_name.c_str(), &active);
	if (framebuffers.size() == 0) {
		Engine::Events::SendNew(new Engine::RequestFramebuffers_Event());
	}
	else {
		float ww = ImGui::GetContentRegionAvail().x;
		ImGui::PushItemWidth(ww / 2 - pad / 2);
		if (ImGui::BeginCombo("##FramebufferStrings", strs[curr_selected])) {
			for (int i = 0; i < num_strs; ++i)
				if (ImGui::Selectable((const char*)strs[i])) {
					curr_selected = i;
					BuildIDStrs();
					break;
				}
			ImGui::EndCombo();
		}
		Engine::Framebuffer& fb = framebuffers[curr_selected];

		if (fb.attachments.size() > 0) {
			ImGui::SameLine();
			ImGui::PushItemWidth(ww / 2 - pad/2);
			if (ImGui::BeginCombo("##IDStringSelection", strs_ids[gl_selected_id])) {
				for (int i = 0; i < fb.attachments.size(); ++i)
					if (ImGui::Selectable((const char*)strs_ids[i])) {
						gl_selected_id = i;
						break;
					}
				ImGui::EndCombo();
			}

			ImVec2 scenesize = { (float)fb.w, (float)fb.h };
			
			scenesize.y = scenesize.y / (fb.w / ww) - pad;
			scenesize.x = ww;

			// TODO: Add framebuffer, renderbuffer, attachment,... selection!
			ImGui::Image((ImTextureID)fb.attachments[gl_selected_id].tex_id, scenesize, { 0,1 }, { 1,0 });
		}
	}
	
	ImGui::End();
}
