#include "RenderTypePeekWindow.h"
#include <src/Application.h>
#include <glew/include/GL/glew.h>

RenderPeekWindow peek_win;


void RenderPeekWindow::Start()
{
}

void RenderPeekWindow::Update()
{
	if (ImGui::CollapsingHeader("Meshes")) {

	}
	if (ImGui::CollapsingHeader("Textures")) {
		static int select_img = 0;
		int num_images = App->renderer3D->textures.size();
		if (num_images > 0) {
			ImGui::SliderInt("##Select", &select_img, 0, num_images - 1);
			const GPUTex& t = App->renderer3D->textures[select_img];
			glEnable(GL_TEXTURE_2D);
			ImGui::Image((ImTextureID)t.img_id, ImVec2(t.w, t.h));
			PLOG("Error initializing OpenGL! %s\n", gluErrorString(glGetError()));
		}
	}
}

void RenderPeekWindow::ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec)
{
}
