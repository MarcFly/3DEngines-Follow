#pragma once
#include "../../ui_item.h"
#include "../../ModuleEngineUI.h"
#include <src/modules/Render/RendererTypes.h>

class SceneView : UI_Item {
	GPUFBO fb;
public:
	SceneView() : UI_Item("Scene View") {};
	void Start();
	void Update();

	void ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec);

	void CleanUp();
};