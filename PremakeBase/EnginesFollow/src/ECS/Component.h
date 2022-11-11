#pragma once

#include "../Globals.h"

namespace Engine {
	struct CID {
		CID(uint64_t _id) : id(_id) {};
		uint64_t ctype = UINT64_MAX;
		uint64_t id = UINT64_MAX;
		uint64_t parent_id = UINT64_MAX;
		uint32_t quick_ref = UINT32_MAX;
	};

	struct Component {
		Component() : cid(PCGRand64()) {}


		CID cid;
		bool active = true;

		virtual void DrawInspector() {};
	};
};

#define COMPONENT_TYPE(name)	static uint64_t type; \
								static uint64_t Register() { type = typeid(name).hash_code(); return type; }; \
								static const char* GetName() { return #name; }; \
								static name invalid;

#define COMPONENT_TYPE_STATIC_DATA(name) uint64_t name::type = 0; \
											name name::invalid;

#define TYPE_STATIC_DATA(name) uint64_t name::type = 0;