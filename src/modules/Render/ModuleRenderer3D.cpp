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
	DDCube, DDCube_BadIndices, DDCube_VecIndices,
	VB_Cube,
};

//====================================
void SetOpenGLState(const OpenGLState& state) {
	SET_STATE(state.depth_test, GL_DEPTH_TEST);
	SET_STATE(state.color_material, GL_COLOR_MATERIAL);
	SET_STATE(state.cull_faces, GL_CULL_FACE);
	SET_STATE(state.lighting, GL_LIGHTING);
	SET_STATE(state.texture2D, GL_TEXTURE_2D);
	glBlendFunc(state.src_blend, state.dst_blend);
}

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module("renderer", start_enabled)
{
	grid_state.lighting = false;
	default_state.src_blend = GL_SRC_ALPHA;
	default_state.dst_blend = GL_ONE_MINUS_SRC_ALPHA;
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
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			PLOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

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
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			PLOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);
	SetOpenGLState(default_state);

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
	// TODO: Camera Matrix part of a Renderdata that can be freely read each frame
	// Will then be useful for splitting the rendering in a different thread
	glLoadMatrixf(App->camera->GetViewMatrix());

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	RenderGrid();
	if (draw_example_primitive) primitive_draw_funs[example_fun]();

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

void ModuleRenderer3D::ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec)
{
	for (std::shared_ptr<Event> ev : evt_vec) {
		switch (ev->type) {
			case EventType::WINDOW_RESIZE:
				OnResize(ev->point2d.x, ev->point2d.y);
				continue;
			case EventType::CHANGED_DEFAULT_OPENGL_STATE:
				default_state = ev->ogl_state;
				SetOpenGLState(default_state);
				continue;
			case EventType::TOGGLE_RENDERER_PRIMITIVES:
				draw_example_primitive = ev->boolean;
				continue;
			case EventType::CHANGE_RENDERER_PRIMITIVE:
				example_fun = ev->uint32;
				continue;
		}
	}
}

#define GRID_SIZE 10

void ModuleRenderer3D::RenderGrid() const
{
	// glDisable(GL_LIGHTING);
	SetOpenGLState(grid_state);

	for (int i = 0; i < GRID_SIZE * 2 + 1; i++)
	{
		glBegin(GL_LINES);
		glColor3f(1.5f, 0.5f, 0.5f);

		//Z
		glVertex3i(GRID_SIZE - i, 0, GRID_SIZE);
		glVertex3i(GRID_SIZE - i, 0, -GRID_SIZE);

		//X
		glVertex3i(-GRID_SIZE, 0, -GRID_SIZE + i);
		glVertex3i(GRID_SIZE, 0, -GRID_SIZE + i);
		glEnd();
	}

	SetOpenGLState(default_state);
	// glEnable(GL_LIGHTING)
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
}
