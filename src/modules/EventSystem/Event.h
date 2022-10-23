#pragma once

#include <stdint.h>
#include <src/modules/render/RendererTypes.h>
#include <src/helpers/JSON/parson.h>
#include <src/modules/ECS/ComponentIncludeAllTypes.h>

#include <string>
enum EventType {
	NO_EVENT,
	// STRING 
	// BOOL 
	// UINT32 
	// POINT 2D
	FILE_DROPPED,
	FS_JSON_PARSED,

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
	
	LOAD_MESH_TO_GPU,
	LOAD_TEX_TO_GPU,
	LOAD_MAT_TO_GPU,
	USER_OFFLOADED_DATA,
	USER_UNLOADED_DATA,
	FRAMEBUFFER_HIJACK,

	ECS_RENDERABLES,
	ECS_REQUEST_NEW_ENTITY,
	ECS_REQUEST_DELETE_ENTITY,
	ECS_REQUEST_MODIFY_ENTITY,
	ECS_ADD_COMPONENT,
	ECS_LOAD_JSONPREFAB,

	HIERARCHY_SELECTED_ENTITY,

	EVENTTYPE_MAX
};

struct Event {
	 EventType type;

	union {
		const char* str;
		bool boolean;
		uint32_t uint32;
		uint64_t uint64;
		struct {
			uint32_t x, y;
		} point2d;
		OpenGLState ogl_state;
		JSON_Object* json_object;
		std::string bad_string;
		void* generic_pointer;
		uint64_t uint64_pair[2];
		uint64_t uint64_trio[3];
		
	};

	Event(EventType _type) : type(_type) {};
	~Event() {}
};


#define EV_SEND_BOOL(type, val) {std::shared_ptr<Event> ev = std::make_shared<Event>(type);\
	ev->boolean = val;\
	App->events->RegisterEvent(ev);}

#define EV_SEND_UINT32(type, val) {std::shared_ptr<Event> ev = std::make_shared<Event>(type);\
	ev->uint32 = val;\
	App->events->RegisterEvent(ev);}

#define EV_SEND_UINT64(type, val) {std::shared_ptr<Event> ev = std::make_shared<Event>(type);\
	ev->uint64 = val;\
	App->events->RegisterEvent(ev);}

// On Multiple data type events, nomenclature should always follow
// ID - Value1 - Value 2 - ...
#define EV_SEND_UINT64_2(type, val1, val2) {std::shared_ptr<Event> ev = std::make_shared<Event>(type);\
	ev->uint64_pair[0] = val1; \
	ev->uint64_pair[1] = val2; \
	App->events->RegisterEvent(ev);}

#define EV_SEND_UINT64_3(type, val1, val2, val3) {std::shared_ptr<Event> ev = std::make_shared<Event>(type);\
	ev->uint64_trio[0] = val1; \
	ev->uint64_trio[1] = val2; \
	ev->uint64_trio[2] = val3; \
	App->events->RegisterEvent(ev);}

#define EV_SEND_POINT2D(type, val1, val2) {std::shared_ptr<Event> ev = std::make_shared<Event>(type);\
	ev->point2d.x = val1;\
	ev->point2d.y = val2;\
	App->events->RegisterEvent(ev);}

#define EV_SEND_JSON_OBJ(type, val) {std::shared_ptr<Event> ev = std::make_shared<Event>(type);\
	ev->json_object = val; \
	App->events->RegisterEvent(ev);}

#define EV_SEND_STR(type, val) {std::shared_ptr<Event> ev = std::make_shared<Event>(type);\
	ev->str = val; \
	App->events->RegisterEvent(ev);}

#define EV_SEND_POINTER(type, val) {std::shared_ptr<Event> ev = std::make_shared<Event>(type); \
	ev->generic_pointer = (void*)val; \
	App->events->RegisterEvent(ev);}