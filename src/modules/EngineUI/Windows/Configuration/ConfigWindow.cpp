#include "ConfigWindow.h"
#include <src/modules/Render/ModuleRenderer3D.h>
#include <src/modules/EventSystem/Event.h>

ConfigWindow cfg_win;
// I rathe have all the variables per menu be local than 

void ConfigWindow::PerformanceGraphs()
{
}

void ConfigWindow::WindowOptions()
{
}

void ConfigWindow::HardwareInfo()
{
}

OpenGLState state;
void ConfigWindow::RenderOptions()
{
	bool ret = false;
	ret |= ImGui::Checkbox("COLOR MATERIAL", &state.color_material);
	ret |= ImGui::Checkbox("CULL FACES", &state.cull_faces);


	if (ret) {
		Event ev(EventType::CHANGED_DEFAULT_OPENGL_STATE);
		ev.ogl_state = state;
	}
}
