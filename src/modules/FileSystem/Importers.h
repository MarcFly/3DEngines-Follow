#pragma once

// Exporters take data that is not in our own format
// converts them to a format we want to use ideally
// then saves them to disk, then asks the disk data to be loaded

#include <src/modules/Module.h>
#include <libs/assimp/scene.h>
#include "FSDataTypes.h"

std::vector<WatchedData> TryImport(const TempIfStream& file, const char* path = nullptr);

bool InitImporters();
bool CleanUpImporters();



bool AssimpInit();
bool AssimpCleanUp();

std::vector<WatchedData> ImportAssimpScene(const TempIfStream& file);

// Meshes
PlainData ImportAssimpMesh(const aiMesh* aimesh);

// Materials
std::vector<WatchedData> ImportAssimpMaterial(const aiMaterial* aimat, const char* parent_path);

// Textures
uint32_t ExtensionToDevILType(const char* ext);
WatchedData TryImportTexture(const TempIfStream& file);
PlainData ImportDevILTexture(const PlainData& pd, uint32_t tex_type);