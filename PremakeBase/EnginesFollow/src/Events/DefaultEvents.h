#pragma once

#include "Events.h"

namespace Engine {
	struct FilesDropped : public Event {
		EVENT_TYPE_CLASS(FilesDropped);
		FilesDropped(const char** _files, int _num) : count(_num) {
			filepaths = new char* [_num];
			for (int i = 0; i < _num; ++i) {
				filepaths[i] = new char[512]; // Standard size paths
				memcpy(filepaths[i], _files[i], strlen(_files[i])+1);
			}
		}
		~FilesDropped() { for (int i = 0; i < count; ++i) delete filepaths[i]; delete filepaths; }
		char** filepaths;
		int count;
	};
}