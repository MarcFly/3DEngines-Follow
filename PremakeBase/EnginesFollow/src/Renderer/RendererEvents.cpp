#include "EnginePCH.h"
#include "Renderer.h"
#include "RendererEvents.h"

using namespace Engine;

EVENT_TYPE_STATIC_DATA(ProvideFramebuffers_Event);
EVENT_TYPE_STATIC_DATA(RequestFramebuffers_Event);

DEF_DYN_MEMBER_EV_FUNS(RequestFramebuffers_Event, Renderer, RequestFramebuffers_EventFun) {
	std::vector<Framebuffer> fbs;
	for (auto& kv_fb : framebuffers) {
		fbs.push_back(kv_fb.second);
	}
	Events::SendNew(new ProvideFramebuffers_Event(fbs));
}}