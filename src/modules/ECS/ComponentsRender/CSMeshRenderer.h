#pragma once
#include <src/Application.h>

#include "../ModuleECS.h"
#include <src/modules/Render/ModuleRenderer3D.h>
#include <src/modules/Render/RendererTypes.h>
#include "../ComponentsMove/CS_Transform.h"
#include "ECS_RendererTypes.h"


struct C_MeshRenderer : public Component {
	constexpr static ComponentTypes type = CT_MeshRenderer;
	C_MeshRenderer() { id.ctype = CT_MeshRenderer; }


	NIMesh mesh_data;
	GPUMesh mesh_use;
	Material mat_data;
	GPUMat mat_use;

	void DrawInspector();

	void PostUpdate(float dt);
};
