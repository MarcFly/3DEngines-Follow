#pragma once

#include "../Globals.h"
#include <parson.h>
#include "Events/Events.h"
#include "Streams.h"

namespace Engine {

	struct FileVirtual {
		uint64_t diskid = UINT64_MAX;
		virtual ~FileVirtual() {};
		virtual void Unload_RAM() {}; // Specific way of unloading memory
		virtual void Load_VRAM() {};
		virtual void Unload_VRAM() {};
		virtual PlainData Serialize() { return PlainData(); }; // If no need to serialize directly, just return a copy of it
		virtual void ParseBytes(TempIfStream& disk_mem) {}; // If no need to parse, acquire memory directly
	};

	struct WatchedData : public FileVirtual {
		WatchedData(FileVirtual*& _mem) :mem(_mem) { _mem = nullptr; }
		WatchedData(WatchedData& _data) {
			id = _data.id;
			ram_users = _data.ram_users;
			vram_users = _data.vram_users;
			loaded_ram = _data.loaded_ram;
			loaded_vram = _data.loaded_vram;
			mem = _data.mem;
			_data.mem = nullptr; 
			path = _data.path;
		}
		uint64_t id = PCGRand64();
		uint32_t ram_users = 0;
		uint32_t vram_users = 0;
		bool loaded_ram = false;
		bool loaded_vram = false;
		double last_check_ts = 0;
		FileVirtual* mem = nullptr;

		std::string path;

		void ParseBytes(TempIfStream& disk_mem);
		PlainData Serialize();

		// Optional: It will first load memory directly from the path file in a binary format
		// If the functions are setup
		inline void Delete_RAM();

		void CheckNecessity();
		void AddUser();
		void RemoveUser();

		~WatchedData() {
			if (mem == nullptr) return;
			if (loaded_ram)
				mem->Unload_RAM();
			if (loaded_vram)
				mem->Unload_VRAM();
			Delete_RAM();
			loaded_ram = loaded_vram = false;
		}
	};

	struct WD_User {
		uint64_t own_id; // user id
		uint64_t data_id = UINT64_MAX; // data being used
	};

	struct FileTaker {
		FileTaker();

		virtual void OnAttach() { Engine_WARN("FileTaker OnAttach undefined"); }
		virtual void OnDetach() { Engine_WARN("FileTaker OnDetach undefined"); }

		// Treating 0 as type not to be loaded by specific FileTaker
		virtual uint32_t ShouldILoad(const char* extension) { return 0; }
		virtual FileVirtual* TryLoad(TempIfStream& raw_bytes, const uint32_t internaltype) { return nullptr; }
	};

	// TODO: project wise, that to change all pointers to shared_ptr/unique_ptr
	// for example some filetakers can be unique_ptr, som can be shared_ptr
	// filevirtuals should all be shared_ptr as they will be passed from hand to hand
	// maybe even weak_ptrs...

	struct FS {
		DECL_STATIC_EV_FUN(TryLoadFiles_EventFun);

		static void Init();
		static void Close();
		
		static uint64_t TryLoadFile(const char* path, const char* parent_path = nullptr);
		static FileVirtual* TryLoad(const char* path, const char* parent_path = nullptr);
		static void WriteToDisk(const char* path, const PlainData& data);
		template<typename T>
		static void RegisterFiletaker(T* send = nullptr) {
			T* curr = send;
			if (curr == nullptr) curr = new T();
			filetakers.push_back(curr);
			curr->OnAttach();
		}

		// This is so bad... need a better way of linking material/textures
		// That will have repercussions further down the line
		// unordered map iterator search is not memory safe
		// it is fucking slow, specially with string search...
		// Could be solved with unique events responses, like, 
		// Send ev with randomid and getting it back with a wait or immediately
		// the UNIQUE system that registered to it, will answer
		static uint64_t FindIDByName(const char* name);

		static std::vector<FileTaker*> filetakers;
		static std::unordered_map<uint64_t, WatchedData> allocs;
		static double unload_bucket_ms; // = 10000;
		static char execpath[512];
		static size_t execpath_len;
	};
}

#define FileTaker_DefaultTryLoad(LoaderType, FileType) Engine::FileVirtual* LoaderType::TryLoad(TempIfStream& raw_bytes, const uint32_t internal_type){\
		FileType* ret = new FileType();\
		ret->ParseBytes(raw_bytes);\
		return ret;}