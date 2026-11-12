/*
 *  render/context.hxx
 *
 *  static render context/state management
 *  cause even in c++, sokol gotta sokol
 */

#ifndef simulator_render_context_hxx
#define simulator_render_context_hxx

#include <core/core.hxx>

#include "framebuffer.hxx"

namespace render {
using namespace core;

enum class InitError {
    ALREADY_INITIALIZED,

    UNKNOWN_INIT_ERROR,
};

enum class CleanupError {
    NOT_INITIALIZED,
    IN_PASS,

    UNKNOWN_CLEANUP_ERROR,
};

enum class PresentError {
    NOT_INITIALIZED,
    IN_PASS,

    UNKNOWN_PRESENT_ERROR,
};

enum class PassError {
    NESTING_PASS,
    NOT_INITIALIZED,

    UNKNOWN_PASS_ERROR,
};

[[nodiscard]] Option<InitError> init();
[[nodiscard]] Option<CleanupError> cleanup();

Option<PresentError> present();

enum class LoadOpt {
    LOAD,
    CLEAR,
};

struct PassOpt {
    LoadOpt loadopt = LoadOpt::CLEAR;
    Vec4 clear_color = {0.05f, 0.1f, 0.2f, 1.0f};
    OptionRef<Framebuffer> framebuffer = {};
};

Option<PassError> pass(Fn<void> cmd, PassOpt pass_opt = {});
} // namespace render

#endif
