#include "../ComponentInspector.h"
#include <src/modules/ECS/ComponentsMove/CS_Transform.h>

bool InspectTransform(Component* c) {
	bool changes_happened = false;
	C_Transform* t = (C_Transform*)c;
	if (ImGui::CollapsingHeader("Transform##" + c->id.id)) {

	}

	return changes_happened;
}