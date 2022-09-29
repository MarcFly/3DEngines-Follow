#include "ConfigWindow.h"
#include <src/modules/Render/RendererTypes.h>
#include <src/modules/EventSystem/Event.h>
#include <src/Application.h>

#include <SDL.h>
#include <libs/glew/include/GL/glew.h>
#include <string>

ConfigWindow cfg_win;
// I rathe have all the variables per menu be local than 

float l1_cache, ram, vram_amount, vram_usage, vram_left, vram_reserved;
int num_cpus;
std::string gpu, gpu_brand;
std::vector<const char*> instructionsets;

void ConfigWindow::Start() {
	l1_cache = SDL_GetCPUCacheLineSize(); // Returns in KB not B
	num_cpus = SDL_GetCPUCount();
	ram = SDL_GetSystemRAM();

	if (SDL_HasAltiVec()) instructionsets.push_back("AltiVec");
	if (SDL_HasAVX()) instructionsets.push_back("AVX");
	if (SDL_HasAVX2()) instructionsets.push_back("AVX2");
	if (SDL_HasMMX()) instructionsets.push_back("MMX");
	if (SDL_HasRDTSC()) instructionsets.push_back("RDTSC");
	if (SDL_HasSSE()) instructionsets.push_back("SSE");
	if (SDL_HasSSE2()) instructionsets.push_back("SSE2");
	if (SDL_HasSSE3()) instructionsets.push_back("SSE3");
	if (SDL_HasSSE41()) instructionsets.push_back("SSE4.2");
	if (SDL_HasSSE42()) instructionsets.push_back("SSE4.1");

	gpu_brand = std::string((const char*)glGetString(GL_VENDOR));
	gpu = std::string((const char*)glGetString(GL_RENDERER));
}

void ConfigWindow::PerformanceGraphs()
{
}

void ConfigWindow::WindowOptions()
{
}


ImVec4 hw_color(1., 1., 0., 1.);

#include "windows.h"
#include "psapi.h"

PROCESS_MEMORY_COUNTERS_EX pmc;

void ConfigWindow::HardwareInfo()
{
	ImGui::Text("CPU Cores: "); ImGui::SameLine(); ImGui::TextColored(hw_color, "%i", num_cpus);
	ImGui::SameLine(); ImGui::TextColored(hw_color, " (%.2f KB L1 Caches / core)", l1_cache);

	
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;


	ImGui::Text("System RAM: "); ImGui::SameLine(); ImGui::TextColored(hw_color, "%.3f GB (%.3f MB used)", ram/1024., (pmc.WorkingSetSize)/(float)(1024*1024));
	ImGui::Text("Instruction Sets: ");
	for (const char* str : instructionsets) {
		ImGui::SameLine();
		ImGui::TextColored(hw_color, "%s", str);
	}
	
	ImGui::Separator();

	ImGui::Text("GPU Brand: "); ImGui::SameLine();
	ImGui::TextColored(hw_color, "%s", gpu_brand.c_str());

	ImGui::Text("GPU Model: "); ImGui::SameLine();
	ImGui::TextColored(hw_color, "%s", gpu.c_str());
}

OpenGLState state;

#include <gl/GL.h>
int curr_src_blend = 0;
int curr_dst_blend = 0;
const char* blend_strings[] = {
	"0", "1", "SRC_COLOR", "1 - SRC_COLOR",
	"DST_COLOR", "1 - DST_COLOR", "SRC_ALPHA",
	"1 - SRC_ALPHA", "DST_ALPHA", "1 - DST_ALPHA",
	"ALPHA_SATURATE"
};
uint32_t src_col_vals[] = {
	GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR,
	GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA,
	GL_SRC_ALPHA_SATURATE,
};

constexpr int arrsize = sizeof(src_col_vals) / sizeof(uint32_t);

void ConfigWindow::RenderOptions()
{
	bool ret = false;
	ret |= ImGui::Checkbox("COLOR MATERIAL", &state.color_material);
	ret |= ImGui::Checkbox("CULL FACES", &state.cull_faces);
	ret |= ImGui::Checkbox("DEPTH TEST", &state.depth_test);
	ret |= ImGui::Checkbox("TEXTURES", &state.texture2D);
	ret |= ImGui::Checkbox("LIGHTING", &state.lighting);
	ImGui::Separator();
	ImGui::Text("Blend Factors:");
	ImGui::Text("Source: "); ImGui::SameLine();
	ret |= ImGui::Combo("##SRCBLEND", &curr_src_blend, blend_strings, arrsize);
	
	ImGui::Text("| Dest: "); ImGui::SameLine();
	ret |= ImGui::Combo("##DSTBLEND", &curr_dst_blend, blend_strings, arrsize);
	
	if (ret) {
		state.src_blend = curr_src_blend;
		state.dst_blend = curr_dst_blend;
		std::shared_ptr<Event> ev = std::make_shared<Event>(CHANGED_DEFAULT_OPENGL_STATE);
		ev->ogl_state = state;
		App->events->RegisterEvent(ev);
	}
}
