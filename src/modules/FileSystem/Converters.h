#pragma once

// Exporters take data that is not in our own format
// converts them to a format we want to use ideally
// then saves them to disk, then asks the disk data to be loaded

#include <src/modules/Module.h>
#include <libs/assimp/scene.h>
#include "FSDataTypes.h"

std::vector<WatchedData> TryConvert(const TempIfStream& file, const char* path = nullptr);
std::vector<WatchedData> TryImport(TempIfStream& file, const char* path = nullptr);
bool InitConverters();
bool CleanUpConverters();



bool AssimpInit();
bool AssimpCleanUp();

std::vector<WatchedData> ConvertAssimpScene(const TempIfStream& file);
WatchedData ImportJsonScene(TempIfStream& file);
// Meshes
PlainData ConvertAssimpMesh(const aiMesh* aimesh);


// Materials
std::vector<WatchedData> ConvertAssimpMaterial(const aiMaterial* aimat, const char* parent_path);


// Textures
uint32_t ExtensionToDevILType_Convert(const char* ext);
WatchedData TryConvertTexture(const TempIfStream& file);
PlainData ConvertDevILTexture(const PlainData& pd, uint32_t tex_type);

uint32_t ExtensionToDevILType_Import(const char* ext);
WatchedData TryImportTexture(const TempIfStream& file);
PlainData ImportDevILTexture(const PlainData& pd, uint32_t tex_type);