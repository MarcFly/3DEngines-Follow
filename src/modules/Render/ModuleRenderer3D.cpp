#include <src/helpers/Globals.h>
#include <src/Application.h>
#include "ModuleRenderer3D.h"
#include <libs/glew/include/GL/glew.h>
#include <libs\SDL\include\SDL_opengl.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glew/lib/Win32/glew32.lib")
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

#include <stdint.h>
#define SET_STATE(enabled, value) if (enabled) glEnable(value); else glDisable(value);
//====================================
typedef void(*VoidFun)();
VoidFun primitive_draw_funs[] = {
	DDTriangle, DDCube, DDCube_BadIndices, DDCube_VecIndices,
	VB_Cube, VBI_Pyramid, VBI_DiskSphere,
};

//====================================
void SetOpenGLState(const OpenGLState& state) {
	SET_STATE(state.depth_test, GL_DEPTH_TEST);
	SET_STATE(state.color_material, GL_COLOR_MATERIAL);
	SET_STATE(state.cull_faces, GL_CULL_FACE);
	SET_STATE(state.lighting, GL_LIGHTING);
	SET_STATE(state.texture2D, GL_TEXTURE_2D);
	glBlendFunc(state.src_blend, state.dst_blend);
	glPolygonMode(state.poly_mode, state.poly_fill);

	PLOG("Check for error: %s\n", gluErrorString(glGetError()));
}

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module("renderer")
{
	grid_state.lighting = false;
	states.push_back(default_state);
	states.push_back(grid_state);
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	PLOG("Creating 3D Renderer context");
	bool ret = true;
	
	// Base Flags
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);

	GLenum err = glewInit();
	PLOG("Using Glew %s", glewGetString(GLEW_VERSION));

	PLOG("Vendor: %s", glGetString(GL_VENDOR));
	PLOG("Renderer: %s", glGetString(GL_RENDERER));
	PLOG("OpenGL version supported %s", glGetString(GL_VERSION));
	PLOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	if(context == NULL)
	{
		PLOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if(ret == true)
	{
		//Use Vsync
		//if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			//PLOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			PLOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			PLOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.1f, 0.1f, 0.1f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			PLOG("Error initializing OpenGL! %s\n", gluErrorString(glGetError()));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		glShadeModel(GL_SMOOTH);
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);
	SetOpenGLState(default_state);
	glEnable(GL_DEBUG_OUTPUT);
	InitPrimitives();

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	SDL_GL_SwapWindow(App->window->window);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);

	glLoadMatrixf(App->camera->GetViewMatrix());

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	PLOG("Incomplete Framebuffer: %s", glewGetErrorString(glGetError()));
	RenderGrid();

	if (draw_example_primitive) primitive_draw_funs[example_fun]();
	glBindTexture(GL_TEXTURE_2D, 0);
	
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	PLOG("Destroying 3D Renderer");
	CleanUpPrimitives();

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Regen Hijacked Framebuffer just in case...
	if (hijack_framebuffer != nullptr) {
		hijack_framebuffer->Destroy();
		hijack_framebuffer->Create(width, height);
	}
}
