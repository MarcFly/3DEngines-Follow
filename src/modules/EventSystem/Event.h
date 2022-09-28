#pragma once

#include <stdint.h>
#include <src/modules/render/ModuleRenderer3D.h>
enum EventType {
	// STRING 
	// BOOL 
	// UINT32 
	// POINT 2D

	CHANGED_DEFAULT_OPENGL_STATE,
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
	};

	Event(EventType _type) : type(_type) {};
};