#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include <src/modules/Module.h>
#include <libs/SDL/include/SDL.h>

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	void ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec);

	void SetTitle(const char* title);

	void Save(JSON_Object* obj);
	void Load(JSON_Object* obj);

public:
	//The window we'll be rendering to
	SDL_Window* window;
	bool fullscreen, full_desktop, resizable, borderless;
	int w = SCREEN_WIDTH * SCREEN_SIZE, h = SCREEN_HEIGHT * SCREEN_SIZE;
	//The surface contained by the window
	SDL_Surface* screen_surface;
};

#endif // __ModuleWindow_H__