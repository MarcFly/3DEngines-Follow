#pragma once

#include "../Component.h"
#include "../ECS.h"
#include "../ECS_Events.h"

#include <MathGeoLib.h>
#include "FileSystem/Loaders/MeshLoader.h"
#include "FileSystem/Loaders/MaterialLoader.h"

namespace Engine {
	struct C_MeshRenderer : public Component {
		COMPONENT_TYPE(C_MeshRenderer);
		float4x4 local_mat = float4x4::identity;
		
		WDHandle<FileMesh> mesh;
		WDHandle<FileMaterial> mat;

		void DrawInspector();
	};

	struct S_MeshRenderer : public System {
		SystemConstruct(S_MeshRenderer, C_MeshRenderer);
		DECL_SystemGenericFuns(S_MeshRenderer, C_MeshRenderer);
		~S_MeshRenderer();

		JSON_Value* JSONValueFromComponent(const Component* c);
		void ComponentFromJSONObject(const JSON_Object* t_obj);
	};
}