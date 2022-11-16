#pragma once

#include "../Globals.h"
#include <parson.h>
#include "Events/Events.h"
#include "Streams.h"

namespace Engine {

	struct FileVirtual {
		uint64_t diskid = UINT64_MAX;
		virtual ~FileVirtual() {};
		virtual void Unload() {}; // Specific way of unloading memory
		virtual void Load(TempIfStream& disk_mem) {}; // It gets the memory it is supposed to use
		virtual PlainData Serialize() { return PlainData(); }; // If no need to serialize directly, just return a copy of it
	};

	struct WatchedData : public FileVirtual {
		WatchedData(std::shared_ptr<FileVirtual>& _mem) : mem(_mem) { if (!mem) loaded = true; }

		uint64_t id = PCGRand64();
		std::vector<uint64_t> users;
		bool loaded = false;
		double last_check_ts = 0;
		std::shared_ptr<FileVirtual> mem;

		std::string path;

		void Load(TempIfStream& disk_mem);
		PlainData Serialize();

		// Optional: It will first load memory directly from the path file in a binary format
		// If the functions are setup
		void Load_FromDisk();

		inline void Delete() {
			if (mem != nullptr) {
				mem->Unload();
				loaded = false;
				mem.reset();
			}
		}

		void CheckNecessity();
		void AddUser();
		void RemoveUser();

		~WatchedData() {
			if (mem == nullptr) return;
			if (loaded)
				mem->Unload();
			Delete();
			loaded = false;
		}
	};

	struct WD_User {
		uint64_t own_id; // user id
		uint64_t data_id = UINT64_MAX; // data being used
	};

	struct FileTaker {
		FileTaker();
		virtual ~FileTaker() {}

		virtual void OnAttach() { Engine_WARN("FileTaker OnAttach undefined"); }
		virtual void OnDetach() { Engine_WARN("FileTaker OnDetach undefined"); }

		// Treating 0 as type not to be loaded by specific FileTaker
		virtual uint32_t ShouldILoad(const char* extension) { return 0; }
		virtual std::shared_ptr<FileVirtual> TryLoad(TempIfStream& raw_bytes, const uint32_t internaltype) { return nullptr; }
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
		static std::shared_ptr<FileVirtual> TryLoad(const char* path, const char* parent_path = nullptr);
		static void WriteToDisk(const char* path, const PlainData& data);
		template<typename T>
		static void RegisterFiletaker(T* send = nullptr) {
			T* curr = send;
			if (curr == nullptr) {
				curr = new T();
				filetakers.push_back(curr);
				curr->OnAttach();
			}
			else {
				not_owned_filetakers.push_back(curr);
			}
		}

		static std::shared_ptr<FileVirtual> AddUser(const uint64_t resource_id);
		static void RemoveUser(const uint64_t resource_id);

		// This is so bad... need a better way of linking material/textures
		// That will have repercussions further down the line
		// unordered map iterator search is not memory safe
		// it is fucking slow, specially with string search...
		// Could be solved with unique events responses, like, 
		// Send ev with randomid and getting it back with a wait or immediately
		// the UNIQUE system that registered to it, will answer
		static uint64_t FindIDByName(const char* name);

		static std::vector<FileTaker*> filetakers;
		static std::vector<FileTaker*> not_owned_filetakers;
		static std::unordered_map<uint64_t, WatchedData> allocs;
		static double unload_bucket_ms;
		static char execpath[512];
		static size_t execpath_len;
	};
	

	// TODO: FileVirtuals, should in itself have another indirection for data inside...
	// New function to be GetDataSharedPtr...
	template<class FileVirtualDerive>
	struct WDHandle {
		uint64_t id;
	
		void Require() {
			if (!mem)
				mem = FS::AddUser(id);
		}
		void Release() {
			FS::RemoveUser(id);
			mem.reset();
		}
	
		std::shared_ptr<FileVirtualDerive> mem;
	};

}

#define FileTaker_DefaultTryLoad(LoaderType, FileType) std::shared_ptr<Engine::FileVirtual> LoaderType::TryLoad(TempIfStream& raw_bytes, const uint32_t internal_type){\
		std::shared_ptr<FileType> ret(new FileType());\
		ret->Load(raw_bytes);\
		return ret;}