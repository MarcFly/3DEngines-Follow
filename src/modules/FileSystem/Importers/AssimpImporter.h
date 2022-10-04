#pragma once

#include <src/modules/Module.h>
#include <libs/assimp/scene.h>
#include "../FSDataTypes.h"
#include <src/modules/Render/RendererTypes.h>

class AssimpImporter : public Module {
public:
	AssimpImporter() : Module("assimp", true) {};
	~AssimpImporter() {}

	bool Init();

	bool CleanUp();

	// Import Functions
	std::vector<WatchedData> ExportAssimpScene(const PlainData& data);
	PlainData ExportAssimpMesh(const aiMesh* aimesh);
};