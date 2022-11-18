#pragma once

#include "Globals.h"
#include "Events/Events.h"
#include "Renderer.h"

#include "FileSystem/Loaders/LoaderFileTypes.h"

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

	struct SubmitDraw_Event : public Event {
		EVENT_TYPE_CLASS(SubmitDraw_Event);

		SubmitDraw_Event(uint64_t id, float4x4& t, WDHandle<FileMesh>& me, WDHandle<FileMaterial>& ma) :
			transform(t), mesh(me), mat(ma), submit_id(id) {}
		uint64_t submit_id;
		float4x4& transform;
		WDHandle<FileMesh>& mesh;
		WDHandle<FileMaterial>& mat;
	};
}