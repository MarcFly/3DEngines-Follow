#pragma once

#include "Globals.h"
#include "Events/Events.h"
#include "Renderer.h"

namespace Engine {
	struct ProvideFramebuffers_Event : public Event {
		EVENT_TYPE_CLASS(ProvideFramebuffers_Event);
		ProvideFramebuffers_Event(const std::vector<Framebuffer> fbs) : framebuffers(fbs) {}
		const std::vector<Framebuffer> framebuffers;
	};

	// TODO: Way to simplify request and provide vents into a single declaration...
	struct RequestFramebuffers_Event : public Event {
		EVENT_TYPE_CLASS(RequestFramebuffers_Event);
		RequestFramebuffers_Event() {}
	};
}