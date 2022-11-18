#pragma once

#include "Globals.h"
#include "../FS.h"
#include "RenderAPI/Render.h"

namespace Engine {

	struct DevILLoader : public FileTaker {
		void OnAttach();
		void OnDetach();
		uint32_t ShouldILoad(const char* extension);
		std::shared_ptr<FileVirtual> TryLoad(TempIfStream& raw_bytes, const uint32_t internaltype);
	};
};