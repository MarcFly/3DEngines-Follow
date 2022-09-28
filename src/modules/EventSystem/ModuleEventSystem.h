#pragma once

#include "../Module.h"
#include "Event.h"
#include <unordered_map>
#include <vector>
#include <memory>

typedef std::vector<std::shared_ptr<Event>> vec_ev;
typedef std::pair < uint32_t, std::vector<std::shared_ptr<Event>>> queue_pair;

class ModuleEventSystem : public Module {
	std::unordered_map <uint32_t, std::vector < std::shared_ptr<Event>>> event_queues;
	// Could be ordered queues with priorities and blah blah blah
public:
	ModuleEventSystem() : Module(true) {};
	~ModuleEventSystem() {};

	bool Init() { return true; };

	void RegisterEvent(std::shared_ptr<Event> ev) {
		vec_ev& vec = event_queues.insert(queue_pair(ev->type, vec_ev())).first->second;
		vec.push_back(ev);
	}
	
};