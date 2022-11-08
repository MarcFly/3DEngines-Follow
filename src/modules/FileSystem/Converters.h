#pragma once

// Exporters take data that is not in our own format
// converts them to a format we want to use ideally
// then saves them to disk, then asks the disk data to be loaded

#include <src/modules/Module.h>
#include <libs/assimp/scene.h>
#include "FSDataTypes.h"
#include <src/modules/Render/RendererTypes.h>

std::vector<WatchedData> TryConvert(const TempIfStream& file, const char* path = nullptr);
bool InitConverters();
bool CleanUpConverters();



bool AssimpInit();
bool AssimpCleanUp();

void ConvertAssimpScene(const TempIfStream& file);
// Meshes
void ConvertAssimpMesh(const aiMesh* aimesh, NIMesh& mesh);

// Materials
void ConvertAssimpMaterial(const aiMaterial* aimat, const char* parent_path, Material& mat);

// Textures
uint32_t ExtensionToDevILType_Convert(const char* ext);
Texture ConvertDevILTexture(const TempIfStream& file);