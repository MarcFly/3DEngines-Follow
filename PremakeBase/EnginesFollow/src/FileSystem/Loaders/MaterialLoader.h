#pragma once

#include "Globals.h"
#include "../FS.h"
#include "RenderAPI/Render.h"
#include "Renderer/Renderer.h"
namespace Engine {

	struct FileMaterial : public FileVirtual {
		std::vector<std::pair<uint64_t, uint32_t>> textures;

		void ParseBytes(TempIfStream& disk_mem);
		PlainData Serialize(); // No serialization for now
	};

	struct MaterialLoader : public FileTaker {
		uint32_t ShouldILoad(const char* extension);
		FileVirtual* TryLoad(TempIfStream& raw_bytes, const uint32_t internaltype);
	};
};