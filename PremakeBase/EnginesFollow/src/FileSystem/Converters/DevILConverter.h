#pragma once

#include "Globals.h"
#include "../FS.h"

namespace Engine {

	struct DevILConverter : public FileTaker {
		void OnAttach();
		void OnDetach();
		uint32_t ShouldILoad(const char* extension);
		FileVirtual* TryLoad(TempIfStream& raw_bytes, const uint32_t internaltype);
	};
};