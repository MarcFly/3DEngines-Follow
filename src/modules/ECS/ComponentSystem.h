#pragma once
#include <src/helpers/Globals.h>
#include <typeinfo>
#include <vector>
#include <queue>

// TODO: Reflection to get the typeids
// If not found, then use the runtime one


template<class SubType, class PackageData>
struct Component {
	bool active = true, socket_active = true;
	SubType component;
	std::vector<PackageData> changes;
	// Changes that have been sent by other components or entities
};

template<class SubType, class PackageData>
class System {
public:
	System() {};
	~System() {};

	// Virtuals
	virtual bool Init() { return true; };

	// On Updates, each component must be updated with the changes sent
	// Ideally ordered
	virtual update_status PreUpdate() { return UPDATE_CONTINUE; };
	virtual update_status Update() { return UPDATE_CONTINUE; };
	virtual update_status PostUpdate() { return UPDATE_CONTINUE; };
	
	virtual bool CleanUp() { return true; };

	// Common Data
	std::vector<Component<SubType, PackageData>> components;

	// Common Functions
	PackageData& PreparePackage(const uint32_t component_handle) {
		Component<SubType, PackageData>& c = components[component_handle]
		c.changes.push_back(PackageData());
		return c.changes.back();
	}

};