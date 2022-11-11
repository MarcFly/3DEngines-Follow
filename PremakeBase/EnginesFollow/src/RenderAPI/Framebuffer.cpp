#include "EnginePCH.h"
#include "Core/Logger.h"
#include <glad/glad.h>
#include "RenderMacros.h"

#include "Render.h"

using namespace Engine;

void Framebuffer::Create() {
	glGenFramebuffers(1, &id);
}

void Framebuffer::AddRenderbuffer(int use_format, int type, uint32_t _w, uint32_t _h) {
	glGenRenderbuffers(1, &renderbuffer_id);
	rb_type = type;
	w = _w; h = _h;
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_id);
	glRenderbufferStorage(GL_RENDERBUFFER, use_format, _w, _h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, type, GL_RENDERBUFFER, renderbuffer_id);
}

void Framebuffer::AddTexAttachment(const uint32_t location, const uint32_t texture_id, const int mipmap_lvl) {
	glFramebufferTexture2D(GL_FRAMEBUFFER, location + GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, mipmap_lvl);
}

void Framebuffer::SetDrawBuffers() {

}

void RenderAPI::Framebuffer_CheckStatus() {
	int fb_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (fb_status) {
	case GL_FRAMEBUFFER_COMPLETE: return;
	case GL_FRAMEBUFFER_UNDEFINED: Engine_WARN("Framebuffer Undefined {}", fb_status); return;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: Engine_WARN("Incomplete Attachment {}", fb_status); return;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: Engine_WARN("Incomplete Draw Buffer {}", fb_status); return;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: Engine_WARN("Incomplete Read Buffer {}", fb_status); return;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: Engine_WARN("Missing Attachment {}", fb_status); return;
	case GL_FRAMEBUFFER_UNSUPPORTED: Engine_WARN("Unsupported {}", fb_status); return;
	}
}

void Framebuffer::Bind() {
	glBindFramebuffer(fb_type, id);

	if(renderbuffer_id > 0)
		glBindRenderbuffer(EF_RBO, id);
}

void Framebuffer::FreeFromGPU() {
	glDeleteFramebuffers(1, &id);
	if (renderbuffer_id > 0)
		glDeleteRenderbuffers(1, &renderbuffer_id);
}