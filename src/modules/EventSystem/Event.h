#pragma once

#include <stdint.h>
#include <src/modules/render/RendererTypes.h>
#include <src/helpers/JSON/parson.h>

enum EventType {
	// STRING 
	// BOOL 
	// UINT32 
	// POINT 2D
	SAVE_CONFIG,
	LOAD_CONFIG,
	ADD_TO_CONFIG,

	WINDOW_RESIZE,
	WINDOW_SET_FULLSCREEN,
	WINDOW_SET_FULL_DESKTOP,
	WINDOW_SET_BORDERLESS,
	WINDOW_SET_RESIZABLE,
	WINDOW_SET_BRIGHTNESS,
	CHANGED_DEFAULT_OPENGL_STATE,
	TOGGLE_RENDERER_PRIMITIVES,
	CHANGE_RENDERER_PRIMITIVE,

	EVENTTYPE_MAX
};

struct Event {
	 EventType type;

	union {
		const char* str;
		bool boolean;
		uint32_t uint32;
		struct {
			uint32_t x, y;
		} point2d;
		OpenGLState ogl_state;
		JSON_Object* json_object;
	};

	Event(EventType _type) : type(_type) {};
};


#define EV_SEND_BOOL(type, val) {std::shared_ptr<Event> ev = std::make_shared<Event>(type);\
	ev->boolean = val;\
	App->events->RegisterEvent(ev);}

#define EV_SEND_UINT32(type, val) {std::shared_ptr<Event> ev = std::make_shared<Event>(type);\
	ev->uint32 = val;\
	App->events->RegisterEvent(ev);}

#define EV_SEND_POINT2D(type, val1, val2) {std::shared_ptr<Event> ev = std::make_shared<Event>(type);\
	ev->point2d.x = val1;\
	ev->point2d.y = val2;\
	App->events->RegisterEvent(ev);}

#define EV_SEND_JSON_OBJ(type, val) {std::shared_ptr<Event> ev = std::make_shared<Event>(type);\
	ev->json_object = val; \
	App->events->RegisterEvent(ev);}