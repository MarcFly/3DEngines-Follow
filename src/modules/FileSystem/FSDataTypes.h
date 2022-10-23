#pragma once
#include <stdint.h>

struct PlainData {
	char* data = nullptr;
	uint64_t size = 0;	
};

template<class T>
inline void SetPlainData(PlainData& pd, T* _data, uint64_t size) {
	pd.data = (char*)_data;
	pd.size = size;
}

template<class T>
inline void AppendVec(std::vector<T>& vec_receiver, std::vector<T>& vec_append) {
	vec_receiver.insert(vec_receiver.end(), std::make_move_iterator(vec_append.begin()), std::make_move_iterator(vec_append.end()));
	vec_append.erase(vec_append.begin(), vec_append.end());
	vec_append.clear();
}

#include <src/helpers/Globals.h>
struct WatchedData {
	uint64_t uid = PCGRand();
	PlainData pd;
	uint32_t users = 0;
	bool loaded = false;
	uint64_t offload_id = UINT64_MAX;
	// Offload Users are the ones that do not care if the memory is in cpu really
	// Like Meshes, that as long as they are buffered will just work
	// The id relates to the othere module that is making use of it

	double last_check_ts = 0;
	uint16_t str_len;
	char* path = nullptr;

	uint32_t load_event_type = 0; // No Event
	uint32_t unload_event_type = 0;
};

struct KeyPosPair {
	uint64_t uid;
	uint64_t data_pos;
};

#include <fstream>
#include <string>
class TempIfStream {
public:
	TempIfStream(const char* _path) {
		TryLoad(_path);		
	};
	~TempIfStream() { CleanUp(); }
	void CleanUp() { if (stream.is_open()) stream.close(); if (pd.data != nullptr) delete[] pd.data; pd.data = nullptr; }
	void TryLoad(const char* _path) {
		CleanUp();
		path = std::string(_path);
		stream.open(path, std::ios::binary);
		if (!stream.fail()) {
			stream.seekg(0, std::ios::end);
			pd.size = stream.tellg();
			stream.seekg(0, std::ios::beg);
			pd.data = new char[pd.size + 1];
			stream.read((char*)pd.data, pd.size);
		}
	}

	const PlainData& GetData() const { return pd; }
	PlainData AcquireData() { PlainData ret = pd; pd.data = nullptr; pd.size = 0; return ret; }

	std::string path;
private:
	std::ifstream stream;
	PlainData pd;	
};


struct DiskFile {
	virtual PlainData LoadPlainData(const TempIfStream& file) { return PlainData(); }
	virtual uint64_t LoadMem(const PlainData& data) { return UINT64_MAX; };
	virtual bool UnloadMem(uint64_t) { return false; };
	virtual PlainData WritableFile() { return PlainData(); };
};