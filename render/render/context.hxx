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
#include "material.hxx"
#include "mesh.hxx"

namespace render {
using namespace core;

#define INIT_ERROR(X)                                                                              \
    X(already_initialized, 1, "already initialized")                                               \
    X(gfx_failed, 2, "sokol gfx failed to init")

DEFINE_ERRCODE(InitError, INIT_ERROR, 1, render)
#undef INIT_ERROR

#define CLEANUP_ERROR(X)                                                                           \
    X(not_initialized, 1, "cleanup when not initialized")                                          \
    X(in_pass, 2, "cleanup when mid-pass")

DEFINE_ERRCODE(CleanupError, CLEANUP_ERROR, 1, render)
#undef CLEANUP_ERROR

#define PRESENT_ERROR(X)                                                                           \
    X(not_initialized, 1, "present when not initialized")                                          \
    X(in_pass, 2, "present while mid-pass")

DEFINE_ERRCODE(PresentError, PRESENT_ERROR, 1, render)
#undef CLEANUP_ERROR

#define PASS_ERROR(X)                                                                              \
    X(not_initialized, 1, "pass when not initialized")                                             \
    X(nesting_pass, 2, "nesting pass")

DEFINE_ERRCODE(PassError, PASS_ERROR, 1, render)
#undef PASS_ERROR

#define DRAW_ERROR(X)                                                                              \
    X(not_initialized, 1, "draw when not initialized")                                             \
    X(not_in_pass, 2, "draw when not in pass")                                                     \
    X(no_vertex_buffer, 3, "draw with no vertex buffer")                                           \
    X(no_index_buffer, 4, "draw with no index buffer")                                             \
    X(no_index_count, 5, "draw with no index count")

DEFINE_ERRCODE(DrawError, DRAW_ERROR, 1, render)
#undef DRAW_ERROR

/**
 * @brief Start the render library.
 *
 * Initialize the render library:
 * - initialize underlying graphics API
 * - prepare internal render state
 *
 * Will fail if called multiple times without a cleanup.
 */
Result<void> init();

/**
 * @brief Shutdown the render library.
 *
 * Clean up the render library:
 * - shutdown underlying graphics API
 * - cleanup render state
 *
 * Will fail if called while not initialized.
 */
Result<void> cleanup();

[[nodiscard]] bool is_active();

Result<void> present();

enum class LoadOpt {
    LOAD,
    CLEAR,
};

struct PassOpt {
    LoadOpt loadopt = LoadOpt::CLEAR;
    Vec4 clear_color = {0.05f, 0.1f, 0.2f, 1.0f};
    Option<Framebuffer &> framebuffer = {};
};

Result<void> pass(Fn<void> cmd, PassOpt pass_opt = {});

Result<void> draw(Material &material, Mesh &mesh);
} // namespace render

#endif
