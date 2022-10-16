#include "SceneView.h"
#include <src/Application.h>
#include <glew/include/GL/glew.h>

void SceneView::Start() {
	fb = App->renderer3D->GenerateScreenFBO();
	EV_SEND_POINTER(FRAMEBUFFER_HIJACK, &fb);
}

void SceneView::Update() {
	ImGui::Begin(name.c_str(), &active);
	ImVec2 scenesize = { (float)fb.attachment.w, (float)fb.attachment.h };
	ImGui::Image((ImTextureID)fb.attachment.img_id, scenesize);
	ImGui::End();
}

void SceneView::ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec)
{

}


void SceneView::CleanUp() {
	glDeleteFramebuffers(1, &fb.framebuffer_id);
	glDeleteTextures(1, &fb.attachment.img_id);
	glDeleteRenderbuffers(1, &fb.renderbuffer_id);

}