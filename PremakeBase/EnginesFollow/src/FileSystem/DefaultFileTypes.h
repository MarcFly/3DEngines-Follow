#pragma once

#include "../Globals.h"
#include <parson.h>
#include "FS.h"
#include <MathGeoLib.h>

namespace Engine {

	struct JSONVWrap : public FileVirtual {
		JSON_Value* value = nullptr;
		void ParseBytes(TempIfStream& disk_mem) {
			value = json_parse_string((char*)disk_mem.GetData().data);
		}
		void Unload_RAM(PlainData& mem) {
			json_value_free(value);
			value = nullptr;
		}

		PlainData Serialize() {
			PlainData ret;
			char* temp = json_serialize_to_string_pretty(value);
			ret.size = strlen(temp);
			ret.data = (byte*)temp;
			return ret;
		}

		~JSONVWrap() { 
			if (value != nullptr) 
				json_value_free(value); 
			value = nullptr;
		}
	};

	
}