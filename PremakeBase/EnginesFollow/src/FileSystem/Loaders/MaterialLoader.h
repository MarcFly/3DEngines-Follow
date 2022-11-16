#pragma once

#include "Globals.h"
#include "../FS.h"
#include "RenderAPI/Render.h"
#include "Renderer/Renderer.h"
#include "DevILLoader.h"

namespace Engine {

	struct FileMaterial : public FileVirtual {
		std::vector<WDHandle<FileTexture>> textures;

		void Load(TempIfStream& disk_mem);
		PlainData Serialize(); // No serialization for now
	};

	struct MaterialLoader : public FileTaker {
		uint32_t ShouldILoad(const char* extension);
		std::shared_ptr<FileVirtual> TryLoad(TempIfStream& raw_bytes, const uint32_t internaltype);
	};
};