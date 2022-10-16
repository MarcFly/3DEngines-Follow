#include "RenderTypePeekWindow.h"
#include <src/Application.h>
#include <glew/include/GL/glew.h>


void RenderPeekWindow::Start()
{
}

void RenderPeekWindow::Update()
{
	ImGui::Begin(name.c_str(), &active);
	
	if (ImGui::CollapsingHeader("Meshes")) {

	}
	if (ImGui::CollapsingHeader("Textures")) {
		ImGui::SliderInt("Texture Show Width", &t_w, 0, 4000);
		static int select_img = 0;
		int num_images = App->renderer3D->textures.size();
		if (num_images > 0) {
			ImGui::SliderInt("##Select", &select_img, 0, num_images - 1);
			const GPUTex& t = App->renderer3D->textures[select_img];
			glEnable(GL_TEXTURE_2D);
			int h = t.h / (t.w / (float)t_w), w = t_w;
			ImGui::Image((ImTextureID)t.img_id, ImVec2(w, h));
			PLOG("Error initializing OpenGL! %s\n", gluErrorString(glGetError()));
		}
	}
	extern GLuint checkers_textureID;
	ImGui::Image((ImTextureID)checkers_textureID, ImVec2(400,400));

	ImGui::End();
}

void RenderPeekWindow::ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec)
{
}
