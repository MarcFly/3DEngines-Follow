#include "EnginePCH.h"

#include "Renderer.h"

using namespace Engine;

//===================================================================
// OPENGL STATE TRACKER
//===================================================================

RenderState::RenderState() { BackUp(); }
RenderState::~RenderState() { SetUp(); }

void RenderState::SetUp()
{
    glBlendEquationSeparate(blend_equation_rgb, blend_equation_alpha);
    if (blend_func_separate)
        glBlendFuncSeparate(blend_func_src_rgb, blend_func_dst_rgb, blend_func_src_alpha, blend_func_dst_alpha);
    else
        glBlendFunc(blend_func_src_alpha, blend_func_dst_alpha);

    if (blend) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
    if (cull_faces) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
    if (depth_test) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
    if (scissor_test) glEnable(GL_SCISSOR_TEST);
    else glDisable(GL_SCISSOR_TEST);

    // OpenGL Version Specific Code
#   ifdef USE_OPENGL
    glPolygonMode(SQUE_FRONT_AND_BACK, (GLenum)polygon_mode[0]);
#   endif

    glBindFramebuffer(EF_DRAW_FBO, draw_framebuffer);
    glBindFramebuffer(EF_READ_FBO, read_framebuffer);
    glBindFramebuffer(EF_FBO, framebuffer);

    glViewport(vp[0], vp[1], vp[2], vp[3]);

    //SQUE_CHECK_RENDER_ERRORS();
}

void RenderState::BackUp()
{
    backed_up = true;

    glGetIntegerv(GL_BLEND_EQUATION_RGB, &blend_equation_rgb);
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &blend_equation_alpha);

    glGetIntegerv(GL_BLEND_SRC_RGB, &blend_func_src_rgb);
    glGetIntegerv(GL_BLEND_DST_RGB, &blend_func_dst_rgb);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &blend_func_src_alpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &blend_func_dst_alpha);

    blend = glIsEnabled(GL_BLEND);
    cull_faces = glIsEnabled(GL_CULL_FACE);
    depth_test = glIsEnabled(GL_DEPTH_TEST);
    scissor_test = glIsEnabled(GL_SCISSOR_TEST);

    // OpenGL Version Specific Code
#   ifdef USE_OPENGL
    glGetIntegerv(GL_POLYGON_MODE, polygon_mode);
#   endif

    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &draw_framebuffer);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &read_framebuffer);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &framebuffer);

    glGetIntegerv(GL_VIEWPORT, vp);

    //SQUE_CHECK_RENDER_ERRORS();
}

//===================================================================
// RENDERER FLOW
//===================================================================

#include "RendererEvents.h"

void Renderer::OnAttach() {
    RenderState backup;

    Framebuffer dummy;
    dummy.fb_type = EF_FBO;
    dummy.Create();
    dummy.Bind();
    dummy.attachments.push_back(Texture());
    Texture& tex = dummy.attachments.back();
    tex.Create();
    tex.dimension_format = GL_TEXTURE_2D;
    tex.Bind();
    tex.w = 1920; tex.h = 1080;
    
    tex.data_format = tex.use_format = GL_RGB;
    tex.var = GL_UNSIGNED_BYTE;
    //tex.AddAttributeI("Mag Filter", GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //tex.AddAttributeI("Min Filter", GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    tex.SendToGPU(NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    tex.ApplyAttributes();
    dummy.AddTexAttachment(0, tex.tex_id);
    
    dummy.AddRenderbuffer(GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, 1920, 1080);

    RenderAPI::Framebuffer_CheckStatus();

    framebuffers.insert(std::pair<uint64_t, Framebuffer>(dummy.id, dummy));

    Events::SubscribeDyn<RequestFramebuffers_Event>(RequestFramebuffers_EventFunRedirect, this);
}

void Renderer::OnDetach() {
    
    //for (std::unordered_map<uint64_t, Framebuffer>::iterator& fb_keys = ++(framebuffers.begin()); fb_keys != framebuffers.end(); ++fb_keys) {
        // TODO:
    //}
}

void Renderer::PreUpdate() {

}

void Renderer::Update() {
   RenderState backup;
   
   framebuffers.begin()->second.Bind();
   Engine::RenderAPI::ClearFB(float4(.5f, .1f, .1f, 1.f));
   
}

void Renderer::PostUpdate() {
    
}