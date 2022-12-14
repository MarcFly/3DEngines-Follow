#include <src/helpers/Globals.h>
#include <src/Application.h>
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow(bool start_enabled) : Module("window")
{
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	PLOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		PLOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		int width = SCREEN_WIDTH * SCREEN_SIZE;
		int height = SCREEN_HEIGHT * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 3.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
		SDL_GL_SetSwapInterval(0);
		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		
		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			PLOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	PLOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

void ModuleWindow::Save(JSON_Object* obj)
{
	json_object_set_boolean(obj, "fullscreen", fullscreen);
	json_object_set_boolean(obj, "borderless", borderless);
	json_object_set_boolean(obj, "full_desktop", full_desktop);
	json_object_set_boolean(obj, "resizable", resizable);
	json_object_set_number(obj, "width", w);
	json_object_set_number(obj, "height", h);
	json_object_set_string(obj, "title", SDL_GetWindowTitle(window));
}


void ModuleWindow::Load(JSON_Object* obj) {
	fullscreen = json_object_get_boolean(obj, "fullscreen");
	borderless = json_object_get_boolean(obj, "borderless");
	full_desktop = json_object_get_boolean(obj, "full_desktop");
	resizable = json_object_get_boolean(obj, "resizable");
	w = json_object_get_number(obj, "width");
	h = json_object_get_number(obj, "height");
	SDL_SetWindowTitle(window, json_object_get_string(obj, "title"));
}