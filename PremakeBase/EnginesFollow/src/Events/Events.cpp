#include <EnginePCH.h>
#include "Events.h"
#include "DefaultEvents.h"
#include "Core/Logger.h"

using namespace Engine;

std::unordered_map<EventID, Events::Subscriber> Events::subscribers;

struct InternalTestEvent : Event {
	EVENT_TYPE_CLASS(InternalTestEvent);
	uint32_t data = 4;
};

DEF_EV_FUN(InternalTestEvent, ExampleEventFun) {
	ev->data;
}}


// from https://stackoverflow.com/questions/7666509/hash-function-for-string
// original http://www.cse.yorku.ca/~oz/hash.html
uint64_t simplehash(const char* str) {
	uint64_t hash = 5381;
	size_t c;
	while (c = *str++)
		hash = ((hash << 5) + hash) + c;

	return hash;
}

//==================================================================
// EVENTS INITIALIZATION
//==================================================================
EVENT_TYPE_STATIC_DATA(FilesDropped);
void Events::Init() {
}



void Events::Close() {
	for (auto& k : subscribers) {
		for (Event* ev : k.second.queue)
			delete ev;
		k.second.queue.clear();
		k.second.responses.clear();
	}
	subscribers.clear();
}
// Returns false if you failed to register the static type
// Needs to call T::Register() before being usable

void Events::DispatchAll() {
	for (auto& key : subscribers) {
		for (Event* ev : key.second.queue) {
			for (EventFun* fun : key.second.responses)
				fun(ev);

			for (std::pair<void*, EventRedirectFun*>& rfun : key.second.redirected_responses)
				rfun.second(ev, rfun.first);

			delete ev;
		}
		key.second.queue.clear();
	}
}