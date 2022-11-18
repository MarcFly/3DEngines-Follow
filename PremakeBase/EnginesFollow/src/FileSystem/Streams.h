#pragma once
#include "../Globals.h"

namespace Engine {
	struct PlainData {
		PlainData() {}
		template<class T>
		PlainData(T*& any, size_t _size) : bytes((byte*)any), size(_size) { any = nullptr; }

		std::shared_ptr<byte[]> bytes;
		size_t size = 0;
	};

	struct ReadStream {
		byte* stream = nullptr;
		uint64_t size = 0;
		byte* pointer = stream;
		uint64_t curr_pos = 0;

		std::vector<uint64_t> reads;

		void SetData(byte* _data, uint64_t _size) {
			pointer = stream = _data;
			size = _size;
			curr_pos = 0;
		}

		template<class T>
		void Get(T* into) {
			reads.push_back(sizeof(T));
			memcpy(into, pointer, reads.back());
			uint64_t oversize = reads.back() + curr_pos;
			if (oversize >= size) {
				curr_pos = oversize - size - size * (oversize / size);
			}
			else {
				curr_pos += reads.back();
			}
			pointer = &stream[curr_pos];
		}

		template<class T>
		T GetV() {
			T ret;
			reads.push_back(sizeof(T));
			memcpy(&ret, pointer, reads.back());
			uint64_t oversize = reads.back() + curr_pos;
			if (oversize >= size) {
				curr_pos = oversize - size - size * (oversize / size);
			}
			else {
				curr_pos += reads.back();
			}
			pointer = &stream[curr_pos];

			return ret;
		}

		template<class T>
		void GetArr(T* into, uint64_t num_elements) {
			reads.push_back(sizeof(T) * num_elements);
			memcpy((void*)into, pointer, reads.back());
			uint64_t oversize = reads.back() + curr_pos;
			if (oversize >= size) {
				curr_pos = oversize - size - size * (oversize / size);
			}
			else {
				curr_pos += reads.back();
			}
			pointer = &stream[curr_pos];
		}
	};

	struct WriteStream {
		byte* stream = nullptr;
		uint64_t size = 0;
		uint64_t capacity = 0;
		byte* pointer = stream;
		uint64_t curr_pos = 0;
		std::vector<uint64_t> writes;

		~WriteStream() {
			if (stream != nullptr) {
				delete[] stream;
				stream = nullptr;
			}
			pointer = nullptr;
			size = capacity = curr_pos = 0;
			writes.clear();
		}

		void Realloc(uint64_t newcap) {
			if (newcap <= capacity) newcap = capacity * 2;
			byte* new_stream = new byte[newcap];
			memcpy(new_stream, stream, size);
			if (stream != nullptr) delete[] stream;
			stream = new_stream;
			pointer = &stream[curr_pos];
			capacity = newcap;
		}

		void Trim() {
			if (size == capacity || size == 0) return;

			byte* trimmed = new byte[size];
			memcpy(trimmed, stream, size);

			if (stream != nullptr) delete stream;
			stream = trimmed;
			pointer = &stream[curr_pos];
			capacity = size;
		}

		template<class T>
		void Add(const T* data) {
			writes.push_back(sizeof(T));
			if (writes.back() + size > capacity) Realloc(writes.back() + size);
			memcpy(pointer, data, writes.back());

			pointer += writes.back();
			curr_pos += writes.back();
			size += writes.back();
		}

		template<class T>
		void AddArr(const T* data, uint64_t num_elements) {
			writes.push_back(sizeof(T) * num_elements);
			if (writes.back() + size > capacity) Realloc(writes.back() + size);
			memcpy(pointer, data, writes.back());
			pointer += writes.back();
			curr_pos += writes.back();
			size += writes.back();
		}

		std::pair<byte*, uint64_t> GetOwnership() {
			Trim();
			byte* ret = stream;
			uint64_t retsize = size;
			size = capacity = curr_pos = 0;
			stream = pointer = nullptr;

			return std::pair<byte*, uint64_t>(ret, retsize);
		}

	};

	class TempIfStream {
	public:
		TempIfStream(const char* _path, const char* parent_path = nullptr) {
			TryLoad(_path, parent_path);
		};
		~TempIfStream() { CleanUp(); }
		void CleanUp() {
			if (stream.is_open()) stream.close();
			if (!bytes) bytes.reset(); size = 0;
		}
		void TryLoad(const char* _path, const char* parent_path = nullptr) {
			CleanUp();
			path = std::string(_path);
			stream.open(path, std::ios::binary);
			if (stream.fail()) {
				if (parent_path == nullptr) return;

				path = std::string(FileNameExt(_path));
				path.insert(0, parent_path);
				stream.open(path, std::ios::binary);
			}
			if (!stream.fail()) {
				stream.seekg(0, std::ios::end);
				size = stream.tellg();
				stream.seekg(0, std::ios::beg);
				bytes = std::shared_ptr<byte[]>(new byte[size]);
				stream.read((char*)bytes.get(), size);
			}
		}

		std::string path;
		std::shared_ptr<byte[]> bytes;
		size_t size;
	private:
		std::ifstream stream;
		
	};
	
};