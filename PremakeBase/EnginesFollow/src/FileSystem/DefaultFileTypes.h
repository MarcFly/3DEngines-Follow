#pragma once

#include "../Globals.h"
#include <parson.h>
#include <MathGeoLib.h>
#include "Streams.h"

namespace Engine {
	struct FileVirtual {
		uint64_t diskid = UINT64_MAX;
		virtual ~FileVirtual() {};
		virtual void Unload() {}; // Specific way of unloading memory
		virtual void Load(TempIfStream& disk_mem) {}; // It gets the memory it is supposed to use
		virtual PlainData Serialize() { return PlainData(); }; // If no need to serialize directly, just return a copy of it
		virtual void LoadMeta(const JSON_Object* meta) {}
		virtual void SaveMeta(JSON_Object* meta) {}
	};

	struct JSONVWrap : public FileVirtual {
		JSON_Value* value = nullptr;
		void Load(TempIfStream& disk_mem) {
			value = json_parse_string((char*)disk_mem.bytes.get());
		}
		void Unload() {
			if (value != nullptr);
				json_value_free(value);
			value = nullptr;
		}

		PlainData Serialize() {
			char* temp = json_serialize_to_string_pretty(value);
			size_t size = strlen(temp);
			return PlainData(temp, size);
		}

		~JSONVWrap() { Unload(); }
	};

	
}