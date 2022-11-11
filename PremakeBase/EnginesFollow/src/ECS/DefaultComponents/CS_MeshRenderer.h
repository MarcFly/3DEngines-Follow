#pragma once

#include "../Component.h"
#include "../ECS.h"
#include "../ECS_Events.h"

#include <MathGeoLib.h>

namespace Engine {
	struct C_MeshRenderer : public Component {
		COMPONENT_TYPE(C_MeshRenderer);
		float4x4 local_mat = float4x4::identity;
		uint64_t diskmesh;
		uint64_t diskmat;

		void DrawInspector();
	};

	struct S_MeshRenderer : public System {
		std::vector<C_MeshRenderer> components;
		SystemConstruct(S_MeshRenderer, C_MeshRenderer);
		DECL_SystemGenericFuns(S_MeshRenderer, C_MeshRenderer);
		~S_MeshRenderer();

	};
}