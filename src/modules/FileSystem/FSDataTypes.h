#pragma once
#include <stdint.h>

struct PlainData {
	char* data = nullptr;
	uint64_t size = 0;	
};

struct WatchedData {
	PlainData pd;
	uint32_t users = 0;
	bool loaded = false;
	uint64_t offload_id = UINT64_MAX;
	// Offload Users are the ones that do not care if the memory is in cpu really
	// Like Meshes, that as long as they are buffered will just work
	// The id relates to the othere module that is making use of it

	double last_check_ts = 0;
	uint16_t str_len;
	char* path;

	uint32_t event_type = 0; // No Event
};

#include <fstream>
class TempIfStream {
public:
	TempIfStream(const char* path) {
		if(pd.data != nullptr) this->~TempIfStream();
		stream.open(path, std::ifstream::binary);
		stream.seekg(0, std::ios::end);
		pd.size = stream.tellg();
		stream.seekg(0, std::ios::beg);
		pd.data = new char[pd.size + 1];
		stream.read((char*)pd.data, pd.size);
	};
	~TempIfStream() { stream.close(); delete[] pd.data; pd.data = nullptr; }
	
	const PlainData& GetData() const { return pd; }
private:
	std::ifstream stream;
	PlainData pd;
};