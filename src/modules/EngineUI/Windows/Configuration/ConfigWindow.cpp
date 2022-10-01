#include "ConfigWindow.h"
#include <src/modules/Render/RendererTypes.h>
#include <src/modules/EventSystem/Event.h>
#include <src/Application.h>

#include <SDL.h>
#include <libs/glew/include/GL/glew.h>
#include <string>

ConfigWindow cfg_win;

ImVec4 hw_color(1., 1., 0., 1.);

// For now bad performance graph
double last_ticks = 0;
double curr_ticks = 0;
double delta_ticks;
std::vector<float> ticks;
int max_ticks = 60;
float fps;
void ConfigWindow::PerformanceGraphs()
{
	curr_ticks = SDL_GetTicks64();
	delta_ticks = curr_ticks - last_ticks;
	last_ticks = curr_ticks;

	ticks.push_back(delta_ticks);
	while(ticks.size() > max_ticks) 
		ticks.erase(ticks.begin());

	fps = 0;
	for (float v : ticks) fps += v;
	fps /= (float)max_ticks;
	fps = 1000. / fps;
	ImGui::PlotHistogram("Frame Info", ticks.data(), ticks.size());

	ImGui::TextColored(hw_color, "%f fps / %.2f ms frametime", fps, delta_ticks);
	ImGui::SliderInt("Num Frames", &max_ticks, 1, 240);
}

int w, h, refresh_rate;
int l_w, l_h;

double brightness;
bool fullscreen, resizable, borderless, full_desktop;

void ConfigWindow::WindowOptions()
{
	bool ret = false;
	if (ImGui::Checkbox("Fullscreen", &fullscreen)) 
		EV_SEND_BOOL(EventType::WINDOW_SET_FULLSCREEN, fullscreen);
	if (ImGui::Checkbox("Borderless", &borderless))
		EV_SEND_BOOL(EventType::WINDOW_SET_BORDERLESS, borderless);
	if (ImGui::Checkbox("Resizable", &resizable))
		EV_SEND_BOOL(EventType::WINDOW_SET_RESIZABLE, resizable);
	if (ImGui::Checkbox("Full Desktop", &full_desktop))
		EV_SEND_BOOL(EventType::WINDOW_SET_FULL_DESKTOP, full_desktop);

	ImGui::SliderInt("Width", &w, 100, 1920);
	ImGui::SliderInt("Height", &h, 100, 1080);
	if(ImGui::Button("ChangeSize"))
		EV_SEND_POINT2D(EventType::WINDOW_RESIZE, w, h)
}




#include "windows.h"
#include "psapi.h"

PROCESS_MEMORY_COUNTERS_EX pmc;

float l1_cache, ram, vram_amount, vram_usage, vram_left, vram_reserved;
int num_cpus;
std::string gpu, gpu_brand;
std::vector<const char*> instructionsets;

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



#include <gl/GL.h>
OpenGLState state;
int curr_src_blend = 0;
int curr_dst_blend = 0;
const char* blend_strings[] = {
	"0", "1", "SRC_COLOR", "1 - SRC_COLOR",
	"DST_COLOR", "1 - DST_COLOR", "SRC_ALPHA",
	"1 - SRC_ALPHA", "DST_ALPHA", "1 - DST_ALPHA",
	"ALPHA_SATURATE"
};
uint32_t blend_vals[] = {
	GL_ZERO, GL_ONE, GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR,
	GL_DST_COLOR, GL_ONE_MINUS_DST_COLOR, GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA, GL_DST_ALPHA, GL_ONE_MINUS_DST_ALPHA,
	GL_SRC_ALPHA_SATURATE,
};

constexpr int arrsize = sizeof(blend_vals) / sizeof(uint32_t);

#include <src/modules/Render/Primitives/Primitives.h>

bool render_primitives = false;
const char* primitive_strs[] = {
	"Direct Draw Cube", "DD Cube Index example", "DD Cube Loop Index",
	"Vertex Array Cube",
};
int curr_primitive = 0;

constexpr int primitive_arrsize = sizeof(primitive_strs) / sizeof(char*);
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

	// Example Primitives
	ret = false;
	if (ImGui::Checkbox("Render Primitive", &render_primitives))
		EV_SEND_BOOL(TOGGLE_RENDERER_PRIMITIVES, render_primitives);

	if (ImGui::Combo("##PrimitiveExamples", &curr_primitive, primitive_strs, primitive_arrsize))
		EV_SEND_UINT32(CHANGE_RENDERER_PRIMITIVE, curr_primitive);
}

//================================================================
// Start defined at the bottom as needs knowledge of used local variables

void ConfigWindow::Start() {
	// Hardware Info
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

	// Window Options
	SDL_GetWindowSize(App->window->window, &w, &h);
	fullscreen = App->window->fullscreen;
	full_desktop = App->window->full_desktop;
	borderless = App->window->borderless;
	resizable = App->window->resizable;


	// Renderer Options
	state = App->renderer3D->default_state;
	for (int i = 0; blend_vals[(curr_src_blend = i)] != state.src_blend; ++i);
	for (int i = 0; blend_vals[(curr_dst_blend = i)] != state.dst_blend; ++i);
	render_primitives = App->renderer3D->draw_example_primitive;
	curr_primitive = App->renderer3D->example_fun;
}

void ConfigWindow::ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec) {
	for (std::shared_ptr<Event> ev: evt_vec) {
		switch (ev->type) {
		case EventType::WINDOW_RESIZE:
			w = ev->point2d.x;
			h = ev->point2d.y;
			continue;
		}
	}
}