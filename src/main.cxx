#include <print>

#include <core/core.hxx>

#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "sokol_log.h"

#include "imgui.h"

#include "sokol_imgui.h"

#include <sol/sol.hpp>

#include <render/render.hxx>

#include "color.h"
#include "test.h"

using namespace core;

// Holder of graphics state objects
struct GfxState {
    render::Framebuffer framebuffer;
    render::Mesh color_mesh;
    render::Mesh white_mesh;
    Shared<render::Pipeline> tex_pipeline;
    Shared<render::Pipeline> color_pipeline;
    render::Material tex_material;
    render::Material color_material;
    Shared<render::Image> image;
    Shared<render::Sampler> sampler;
};

static Option<GfxState> gfx = {};

// Holder of Lua + console state
struct lua_state_t {
    std::array<char, 512> console_buf;
    std::vector<std::string> console_out;
    sol::state state;
} lua;

static void lua_print(sol::variadic_args va);
static void reset_lua_state();
static void show_console();
static void init(void);
static void frame(void);
static void event(const sapp_event *event);
static void cleanup(void);

// Custom Lua print function to print to the UI console rather than stdout
static void lua_print(sol::variadic_args va) {
    std::string str;
    for (auto v : va) {
        // use Lua's tostring to get a string, then convert to std::string
        auto vstr = lua.state["tostring"](v.as<sol::object>()).get<std::string>();
        str += vstr + "\t";
    }
    lua.console_out.push_back(str);
}

// (Re)start Lua state and prepare environment
static void reset_lua_state() {
    // Reset state
    lua.state = sol::state();
    // Open only core libraries (no I/O, OS, etc.)
    lua.state.open_libraries(sol::lib::base, sol::lib::math, sol::lib::string);
    // Override print with custom one for console output
    lua.state["print"] = lua_print;
}

// Display the Lua console with multi-line input, output, and controls
static void show_console() {
    if (ImGui::Begin("Lua Test Console")) {
        // World's worst code editor
        ImGui::InputTextMultiline("Script Input", lua.console_buf.data(), lua.console_buf.size(),
                                  ImVec2(0, 0), ImGuiInputTextFlags_AllowTabInput);

        // Character counter
        std::string_view str(lua.console_buf.data());
        ImGui::Text("%zu/%zu", str.size(), lua.console_buf.size());

        // Safely execute script and output errors to the console
        if (ImGui::Button("Execute")) {
            auto res = lua.state.safe_script(str, sol::script_pass_on_error);
            if (!res.valid()) {
                sol::error err = res;
                lua.console_out.push_back(std::string("[error] ") + err.what());
            }
        }

        ImGui::SameLine();

        // Globals persist between script executions, resetting clears them
        if (ImGui::Button("Reset Lua"))
            reset_lua_state();

        ImGui::SameLine();

        if (ImGui::Button("Clear Output"))
            lua.console_out.clear();

        ImGui::Separator();

        // Formatted output, each message is a separate widget
        if (ImGui::BeginChild("Output", ImVec2(0, ImGui::GetContentRegionAvail().y),
                              ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar)) {
            for (auto i : lua.console_out) {
                if (i.starts_with("[error]"))
                    ImGui::TextColored({1.0f, 0.4f, 0.2f, 1.0f}, "%s", i.c_str());
                else
                    ImGui::Text("%s", i.c_str());
            }

            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();
    }
    ImGui::End();
}

static void init(void) {
    render::init().expect("[RENDER] Init Error");

    auto tex_pipeline = Shared<render::Pipeline>::create(
        render::Pipeline::create(PIPELINE_CREATE_ARGS(test, test, sg_query_backend())).unwrap());
    auto color_pipeline = Shared<render::Pipeline>::create(
        render::Pipeline::create(PIPELINE_CREATE_ARGS(color, color, sg_query_backend())).unwrap());
    GfxState state = GfxState{
        .framebuffer = render::Framebuffer::create({512, 512}).unwrap(),
        .color_mesh = render::Mesh::create_quad(Vec3(-0.5f, -0.5f, 0), Vec2(1.f), true).unwrap(),
        .white_mesh = render::Mesh::create_quad(Vec3(-1.f, -1.f, 0), Vec2(2.f), false).unwrap(),
        .tex_pipeline = tex_pipeline,
        .color_pipeline = color_pipeline,
        .tex_material = render::Material::create(tex_pipeline).unwrap(),
        .color_material = render::Material::create(color_pipeline).unwrap(),
        .image = std::make_shared<render::Image>(render::Image::create_xor().unwrap()),
        .sampler = std::make_shared<render::Sampler>(
            render::Sampler::create(render::Sampler::Filter::NEAREST,
                                    render::Sampler::Border::BLACK, render::Sampler::Wrap::CLAMP)
                .unwrap()),
    };
    gfx.emplace(std::move(state));

    simgui_desc_t imgui_desc{};
    imgui_desc.logger.func = slog_func;
    simgui_setup(&imgui_desc);

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::GetIO().Fonts->AddFontDefaultVector();

    reset_lua_state();
}

static void frame(void) {
    simgui_frame_desc_t imgui_fdesc{
        .width = sapp_width(),
        .height = sapp_height(),
        .delta_time = sapp_frame_duration(),
        .dpi_scale = sapp_dpi_scale(),
    };
    simgui_new_frame(&imgui_fdesc);

    ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::ShowDemoWindow();

    show_console();

    if (ImGui::Begin("Viewport")) {
        f32 ar = gfx->framebuffer.get_size().y / gfx->framebuffer.get_size().x;
        Vec2 region = ImGui::GetContentRegionAvail();
        f32 s = std::min(region.x / ar, region.y);
        Vec2 size(s * ar, s);
        Vec2 pos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(pos + (region - size) * 0.5f);
        ImGui::Image(simgui_imtextureid(gfx->framebuffer._get_color()->_get_tex_view()),
                     ImVec2(s * ar, s));
    }
    ImGui::End();

    render::PassOpt frame_opt{
        .clear_color = Vec4{0.5f, 0.5f, 0.5f, 1.0f},
          .framebuffer = gfx->framebuffer
    };
    render::pass(
        []() {
            f32 t = sapp_frame_count() / 120.0f;
            f32 st = sinf(t);
            f32 ct = cosf(t);
            Mat2 ar = Mat2(gfx->framebuffer.get_size().y / gfx->framebuffer.get_size().x, 0, 0, 1);

            Mat4 rot_mvp = Mat4(ar * Mat2(ct, st, -st, ct));
            gfx->color_material.set_uniform(rot_mvp, "vs_params", "mvp");

            render::draw(gfx->color_material, gfx->color_mesh).unwrap();

            Mat4 scale_mvp = Mat4(0.5f * ar * Mat2(st));
            gfx->tex_material.set_texture(gfx->image, "tex0");
            gfx->tex_material.set_sampler(gfx->sampler, "smp0");
            gfx->tex_material.set_uniform(scale_mvp, "vs_params", "mvp");

            render::draw(gfx->tex_material, gfx->white_mesh).unwrap();
        },
        frame_opt)
        .expect("[RENDER] Pass Error");

    render::pass([]() { simgui_render(); }).expect("[RENDER] Pass Error");

    render::present().expect("[RENDER] Present Error");
}

static void cleanup(void) {
    simgui_shutdown();

    gfx.reset();

    if (const auto result = render::cleanup(); !result.has_value()) {
        std::println("[RENDER] Cleanup Error: {}", result.error().message());
        sapp_quit();
        return;
    }
}

static void event(const sapp_event *event) {
    simgui_handle_event(event);
    if (event->type == SAPP_EVENTTYPE_KEY_DOWN) {
        if (event->modifiers & SAPP_MODIFIER_ALT && event->key_code == SAPP_KEYCODE_ENTER)
            sapp_toggle_fullscreen();
        if (event->modifiers & SAPP_MODIFIER_ALT && event->key_code == SAPP_KEYCODE_Q)
            sapp_request_quit();
    }
}

sapp_desc sokol_main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    return sapp_desc{
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .event_cb = event,
        .width = 1920,
        .height = 1080,
        .sample_count = 1,
        .high_dpi = true,
        .window_title = "Simulator",
        .icon = {.sokol_default = true},
        .logger = {.func = slog_func},
        .html5 = {.ask_leave_site = true},
    };
}
