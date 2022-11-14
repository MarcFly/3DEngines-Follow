#pragma once

#include "../Component.h"
#include "../ECS.h"
#include "../ECS_Events.h"

#include <MathGeoLib.h>

namespace Engine {
	struct C_Transform : public Component {
		COMPONENT_TYPE(C_Transform);
		float4x4 world_mat = float4x4::identity;	// What needs to multiply local to make it world space
		float4x4 local_mat = float4x4::identity;	// Local transformation of the object itself
		bool is_static = true;
		void DrawInspector();
	};
	

	struct S_Transform : public System {
		SystemConstruct(S_Transform, C_Transform);
		DECL_SystemGenericFuns(S_Transform, C_Transform);
		~S_Transform();


		// Update(float dt);

		// Usage

		//Component* GetComponent(CID& cid);

		//void DeleteComponent(const CID& cid);

		JSON_Value* JSONValueFromComponent(const Component* c);
		void ComponentFromJSONObject(const JSON_Object* t_obj);
		// Custom

		//void SetWorldMatrix(C_Transform& t);

	};
	
};