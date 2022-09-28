#pragma once
#include <src/modules/Module.h>
#include <src/helpers/Globals.h>
#include <src/helpers/glmath.h>
#include <vector>
#define MAX_LIGHTS 8


struct OpenGLState {
	bool lighting = true;
	bool cull_faces = true;
	bool depth_test = true;
	bool color_material = true;
	bool texture2D = true;

	uint32_t src_alpha, dst_alpha;
	uint32_t src_color, dst_color;
};

void SetOpenGLState(const OpenGLState& state);

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnResize(int width, int height);

	void RenderGrid() const;

public:

	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

	const char* glsl_version = "#version 330";

	// State
	OpenGLState default_state;
	OpenGLState grid_state;
	std::vector<OpenGLState> state_stack;
	std::vector<OpenGLState> states;
};