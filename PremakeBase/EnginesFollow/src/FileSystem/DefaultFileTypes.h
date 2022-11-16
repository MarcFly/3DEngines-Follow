#pragma once

#include "../Globals.h"
#include <parson.h>
#include "FS.h"
#include <MathGeoLib.h>

namespace Engine {

	struct JSONVWrap : public FileVirtual {
		JSON_Value* value = nullptr;
		void Load(TempIfStream& disk_mem) {
			value = json_parse_string((char*)disk_mem.bytes.get());
		}
		void Unload_RAM(PlainData& mem) {
			json_value_free(value);
			value = nullptr;
		}

		PlainData Serialize() {
			char* temp = json_serialize_to_string_pretty(value);
			size_t size = strlen(temp);
			return PlainData(temp, size);
		}

		~JSONVWrap() { 
			if (value != nullptr) 
				json_value_free(value); 
			value = nullptr;
		}
	};

	
}