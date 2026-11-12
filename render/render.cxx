/*
 *  render/render.cxx
 *
 *  primary render lib source
 */

#include "render/render.hxx"

#include <sokol_gfx.h>
#include <sokol_glue.h>
#include <sokol_log.h>

struct _RenderState {
    bool initialized = false;
    bool in_pass = false;
};

static _RenderState _render_state;

using namespace core;

namespace render {
Option<InitError> init() {
    if (_render_state.initialized)
        return InitError::ALREADY_INITIALIZED;
    _render_state.initialized = true;
    _render_state.in_pass = false;

    sg_desc gfx_desc{};
    gfx_desc.environment = sglue_environment();
    gfx_desc.logger.func = slog_func;
    sg_setup(&gfx_desc);

    return {};
}

Option<CleanupError> cleanup() {
    if (!_render_state.initialized)
        return CleanupError::NOT_INITIALIZED;
    if (_render_state.in_pass)
        return CleanupError::IN_PASS;

    sg_shutdown();
    _render_state.initialized = false;

    return {};
}

Option<PresentError> present() {
    if (!_render_state.initialized)
        return PresentError::NOT_INITIALIZED;
    if (_render_state.in_pass)
        return PresentError::IN_PASS;

    sg_commit();

    return {};
}

Option<PassError> pass(Fn<void> cmd, PassOpt pass_opt) {
    if (!_render_state.initialized)
        return PassError::NOT_INITIALIZED;
    if (_render_state.in_pass)
        return PassError::NESTING_PASS;

    sg_pass pass = {};
    if (pass_opt.framebuffer.has_value()) {
        Framebuffer &framebuffer = pass_opt.framebuffer.value();
        pass.attachments.colors[0] = framebuffer._get_color()._get_att_view();
        pass.attachments.depth_stencil = framebuffer._get_depth()._get_att_view();
    } else {
        pass.swapchain = sglue_swapchain();
    }
    pass.action.colors[0].load_action =
        pass_opt.loadopt == LoadOpt::CLEAR ? SG_LOADACTION_CLEAR : SG_LOADACTION_LOAD;
    pass.action.colors[0].clear_value = {pass_opt.clear_color.r, pass_opt.clear_color.g,
                                         pass_opt.clear_color.b, pass_opt.clear_color.a};

    _render_state.in_pass = true;
    sg_begin_pass(pass);
    cmd();
    sg_end_pass();
    _render_state.in_pass = false;

    return {};
}
} // namespace render
