#pragma once

// Exporters take data that is not in our own format
// converts them to a format we want to use ideally
// then saves them to disk, then asks the disk data to be loaded

#include <src/modules/Module.h>
#include <libs/assimp/scene.h>
#include "FSDataTypes.h"
#include <src/modules/Render/RendererTypes.h>

bool InitExporters();
bool CleanUpExporters();

std::vector<WatchedData> TryExport(TempIfStream& file, const char* path);

bool AssimpInit();
bool AssimpCleanUp();

std::vector<WatchedData> ExportAssimpScene(const PlainData& data);

// Meshes
PlainData ExportAssimpMesh(const aiMesh* aimesh);

// Materials
std::vector<WatchedData> ExportAssimpMaterial(const aiMaterial* aimat);

// Textures
uint32_t ExtensionToDevILType(const char* ext);
PlainData ExportDevILTexture(const PlainData& pd, uint32_t tex_type);