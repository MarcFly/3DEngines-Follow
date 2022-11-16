#pragma once

#include "Globals.h"
#include "../FS.h"

struct aiLogStream;
struct aiMaterial;
struct aiMesh;

namespace Engine {

	struct AssimpConverter : public FileTaker {
		void OnAttach();
		void OnDetach();
		uint32_t ShouldILoad(const char* extension);
		std::shared_ptr<FileVirtual> TryLoad(TempIfStream& raw_bytes, const uint32_t internaltype);

		uint64_t ConvertMaterial(const aiMaterial* aimat, const char* parent_path);
		uint64_t ConvertMesh(const aiMesh* aimesh, const char* filename, int meshnum);

		aiLogStream* stream = nullptr;
	};
};