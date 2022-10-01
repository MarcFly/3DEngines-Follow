#pragma once

#include "../Module.h"
#include "Event.h"
#include <unordered_map>
#include <vector>
#include <memory>

typedef std::vector<std::shared_ptr<Event>> vec_ev;
typedef std::pair < uint32_t, std::vector<std::shared_ptr<Event>>> queue_pair;

class ModuleEventSystem : public Module {
	// Idea for multithreading - Different queues per type that can be send and no race condition
	// std::unordered_map <uint32_t, std::vector < std::shared_ptr<Event>>> event_queues;
	// Could be ordered queues with priorities and blah blah blah
	std::vector<std::shared_ptr<Event>> event_vec;
	std::vector<std::shared_ptr<Event>> buffer_vec;
	// Before sending events, we check buffer vec is cleared
	// Then lock it, swap with event_vec // When multithreading
	// Send event_vec
	// Create own safe_vec / safe_paged_vec / ...
	// Basically has a ReadWriteLock for push operations...

public:
	ModuleEventSystem() : Module("eventsys", true) {};
	~ModuleEventSystem() {};

	bool Init() { return true; };

	void SendEvents();

	update_status PreUpdate(float dt) {
		SendEvents();
		return UPDATE_CONTINUE;
	}

	update_status Update(float dt) {
		SendEvents();
		return UPDATE_CONTINUE;
	}

	update_status PostUpdate(float dt) {
		SendEvents();
		return UPDATE_CONTINUE;
	}

	void RegisterEvent(std::shared_ptr<Event> ev) {
		// Idea for multithreading? Different queues and needed when register based events
		// For mass sending it does not matter really
		// vec_ev& vec = event_queues.insert(queue_pair(ev->type, vec_ev())).first->second;
		// vec.push_back(ev);
		event_vec.push_back(ev);
	}
	
};