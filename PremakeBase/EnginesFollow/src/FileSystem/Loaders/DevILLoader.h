#pragma once

#include "Globals.h"
#include "../FS.h"
#include "RenderAPI/Render.h"

namespace Engine {

	struct FileTexture : public FileVirtual {
		PlainData bytes;
		Texture gputex;
		uint32_t deviltype;

		void Unload();
		void Load(TempIfStream& disk_mem);
		// PlainData Serialize(); // No serialization for now
	};

	struct DevILLoader : public FileTaker {
		void OnAttach();
		void OnDetach();
		uint32_t ShouldILoad(const char* extension);
		std::shared_ptr<FileVirtual> TryLoad(TempIfStream& raw_bytes, const uint32_t internaltype);
	};
};