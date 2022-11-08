#pragma once
#include <src/modules/Module.h>
#include <src/helpers/Globals.h>
#include <src/helpers/MathGeoLib/MathGeoLib.h>
#include <src/helpers/glmath.h>
#include <vector>
#include <unordered_map>
#include "RendererTypes.h"
#include "Primitives/Primitives.h"
#include <SDL/include/SDL.h>

#define MAX_LIGHTS 8

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
	int example_fun = 0;
	bool draw_example_primitive = false;

	std::unordered_map<uint64_t, GPUMesh> meshes;
	std::unordered_map<uint64_t, GPUTex> textures;
	std::unordered_map<uint64_t, GPUMat> materials;

	GPUFBO* hijack_framebuffer = nullptr;
};