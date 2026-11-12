// source file to create implementation of sokol in static library form

#ifdef __EMSCRIPTEN__
// build GLES3 on Emscripten to target WebGL2
#define SOKOL_GLES3
#else
// build Vulkan elsewhere because its cool
#define SOKOL_VULKAN
#endif

// implement all sokol libraries
#define SOKOL_IMPL
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_log.h"

#include "imgui.h"
#define SOKOL_IMGUI_IMPL
#include "sokol_imgui.h"
