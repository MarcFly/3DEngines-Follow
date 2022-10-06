#pragma once

#include "../../ui_item.h"
#include "../../ModuleEngineUI.h"

class RenderPeekWindow : UI_Item {
public:
	RenderPeekWindow() : UI_Item("PeekRenderData") {};
	void Start();

	void Update();

	void ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec);
};