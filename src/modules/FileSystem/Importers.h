#pragma once

#include <src/modules/Module.h>
#include <libs/assimp/scene.h>
#include "FSDataTypes.h"
#include <src/modules/Render/RendererTypes.h>

bool AssimpInit();
bool AssimpCleanUp();

std::vector<WatchedData> ExportAssimpScene(const PlainData& data);

// Meshes
PlainData ExportAssimpMesh(const aiMesh* aimesh);

// Materials
std::vector<WatchedData> ExportAssimpMaterial(const aiMaterial* aimat);

// Textures
uint32_t ExtensionToDevILType(const char* ext);
PlainData ImportDevILTexture(const PlainData& pd, uint32_t tex_type);