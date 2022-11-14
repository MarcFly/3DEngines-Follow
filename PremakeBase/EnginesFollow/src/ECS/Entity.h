#pragma once

#include "../Globals.h"
#include "Component.h"

namespace Engine {
	struct Entity {		
		static uint64_t default_name;

		bool active = true;
		uint64_t id = PCGRand64();
		uint64_t parent_id = UINT64_MAX;
		std::vector<CID> components;
		std::vector<uint64_t> children;

		char name[32] = "";

		void Refresh(); // Updates Parenting and components

		inline std::vector<CID> GetComponentsOfType(const uint64_t type) {
			std::vector<CID> ret;
			for (CID& cid : components)
				if (cid.ctype == type)
					ret.push_back(cid);
			return ret;
		}
		template<typename T>
		inline std::vector<CID> GetComponents() { return GetComponentsOfType(T::type); }
	};
};