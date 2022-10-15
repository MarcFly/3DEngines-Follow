#pragma once

#include "../ComponentSystem.h"

struct MeshRenderer_Package {

};

struct MeshRenderer {

};

class System_MeshRenderer : System<MeshRenderer, MeshRenderer_Package> {
public:
	System_MeshRenderer() {};
};

extern System_MeshRenderer mesh_renderer;
