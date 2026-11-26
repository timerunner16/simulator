/*
 *  render/render.cxx
 *
 *  primary render lib source
 */

#include "render/render.hxx"

#include <sokol_gfx.h>
#include <sokol_glue.h>
#include <sokol_log.h>

#include <algorithm>
#include <bit>
#include <iterator>

struct _RenderState {
    bool initialized = false;
    bool in_pass = false;
    sg_environment env = {};
};

static _RenderState _render_state;

using namespace core;

namespace render {
Result<void> init() {
    if (_render_state.initialized)
        return Err(InitError::already_initialized);
    _render_state.initialized = true;
    _render_state.in_pass = false;
    _render_state.env = sglue_environment();

    sg_desc gfx_desc{};
    gfx_desc.environment = _render_state.env;
    gfx_desc.logger.func = slog_func;
    sg_setup(&gfx_desc);

    if (!sg_isvalid())
        return Err(InitError::gfx_failed);

    return {};
}

Result<void> cleanup() {
    if (!_render_state.initialized)
        return Err(CleanupError::not_initialized);
    if (_render_state.in_pass)
        return Err(CleanupError::in_pass);

    sg_shutdown();
    _render_state.initialized = false;

    return {};
}

bool is_active() {
    return _render_state.initialized;
}

Result<void> present() {
    if (!_render_state.initialized)
        return Err(PresentError::not_initialized);
    if (_render_state.in_pass)
        return Err(PresentError::in_pass);

    sg_commit();

    return {};
}

Result<void> pass(Fn<void> cmd, PassOpt pass_opt) {
    if (!_render_state.initialized)
        return Err(PassError::not_initialized);
    if (_render_state.in_pass)
        return Err(PassError::nesting_pass);

    sg_pass pass = {};
    if (pass_opt.framebuffer.has_value()) {
        Framebuffer &framebuffer = pass_opt.framebuffer.value();
        pass.attachments.colors[0] = framebuffer._get_color()->_get_att_view();
        pass.attachments.depth_stencil = framebuffer._get_depth()->_get_att_view();
        sg_apply_viewportf(0, 0, framebuffer.get_size().x, framebuffer.get_size().y, false);
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

Result<void> draw(Material &material, Mesh &mesh) {
    if (!_render_state.initialized)
        return Err(DrawError::not_initialized);
    if (!_render_state.in_pass)
        return Err(DrawError::not_in_pass);
    if (!mesh._get_vertex_buffer().id)
        return Err(DrawError::no_vertex_buffer);
    if (!mesh._get_index_buffer().id)
        return Err(DrawError::no_index_buffer);
    if (!mesh._get_index_count())
        return Err(DrawError::no_index_count);
    material._get_pipeline()._apply_pipeline();
    material._apply_uniforms();
    sg_bindings bindings = material._get_bindings();
    bindings.vertex_buffers[0] = mesh._get_vertex_buffer();
    bindings.index_buffer = mesh._get_index_buffer();
    sg_apply_bindings(bindings);

    sg_draw(0, mesh._get_index_count(), 1);

    return {};
}

_Uniforms _Uniforms::create(const _ProgramLayout &program_layout) {
    Vector<Shared<Image>> tex_refs;
    Vector<Shared<Sampler>> smp_refs;

    for (auto &[tex_name, tex_desc] : program_layout.get_tex_map())
        tex_refs.push_back(nullptr);
    for (auto &[smp_name, smp_desc] : program_layout.get_smp_map())
        smp_refs.push_back(nullptr);

    Vector<u8> ub_data;
    auto ub_map = program_layout.get_ub_map();
    Vector<_ubDesc> ub_vec;
    std::transform(ub_map.begin(), ub_map.end(), std::back_inserter(ub_vec),
                   [](auto p) { return p.second; });
    std::sort(ub_vec.begin(), ub_vec.end(), [](_ubDesc a, _ubDesc b) { return a.slot < b.slot; });
    Vector<usize> ub_offsets(ub_vec.size());
    Vector<usize> ub_sizes(ub_vec.size());

    usize offset = 0;
    for (auto ub_desc : ub_vec) {
        ub_data.resize(ub_data.size() + ub_desc.size, 0);
        ub_offsets[ub_desc.slot] = offset;
        ub_sizes[ub_desc.slot] = ub_desc.size;
        offset += ub_desc.size;
    }

    return _Uniforms(tex_refs, smp_refs, ub_data, ub_offsets, ub_sizes, program_layout);
}

Result<Material> Material::create(Shared<Pipeline> pipeline) {
    if (!pipeline)
        return Err(MaterialError::bad_pipeline);
    const _ProgramLayout &layout = pipeline->_get_layout();

    _Uniforms uniforms = _Uniforms::create(layout);

    return Material(pipeline, uniforms);
}

Result<Image> Image::create_fill(const Vec2 &dims, u8 r, u8 g, u8 b, u8 a) {
    if (dims.x <= 0 || dims.y <= 0)
        return Err(ImageError::invalid_dimensions);

    std::vector<u32> data(dims.x * dims.y, std::bit_cast<u32, u8[4]>({r, g, b, a}));

    sg_image_desc image_desc = {};
    image_desc.width = dims.x;
    image_desc.height = dims.y;
    image_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    image_desc.sample_count = 1;
    image_desc.data = {
        sg_range{.ptr = data.data(), .size = 4 * data.size()}
    };
    const sg_image image = sg_make_image(image_desc);
    if (sg_query_image_state(image) != SG_RESOURCESTATE_VALID) {
        sg_destroy_image(image);
        return Err(ImageError::image_failed);
    }

    sg_view_desc tex_view_desc = {};
    tex_view_desc.texture.image = image;
    const sg_view tex_view = sg_make_view(tex_view_desc);
    if (sg_query_view_state(tex_view) != SG_RESOURCESTATE_VALID) {
        sg_destroy_view(tex_view);
        sg_destroy_image(image);
        return Err(ImageError::view_failed);
    }

    return Image(Unique<const sg_image>::create(image), nullptr,
                 Unique<const sg_view>::create(tex_view), dims);
}

Result<Image> Image::create_xor() {
    std::vector<u8> data(4 * 256 * 256);
    for (i32 y = 0; y < 256; y++) {
        for (i32 x = 0; x < 256; x++) {
            usize id = (y * 256 + x) * 4;
            u8 c = x ^ y;

            data[id] = c;
            data[id + 1] = c;
            data[id + 2] = c;
            data[id + 3] = 255;
        }
    }

    sg_image_desc image_desc = {};
    image_desc.width = 256;
    image_desc.height = 256;
    image_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
    image_desc.data = {
        sg_range{.ptr = data.data(), .size = data.size()}
    };
    const sg_image image = sg_make_image(image_desc);
    if (sg_query_image_state(image) != SG_RESOURCESTATE_VALID) {
        sg_destroy_image(image);
        return Err(ImageError::image_failed);
    }

    sg_view_desc tex_view_desc = {};
    tex_view_desc.texture.image = image;
    const sg_view tex_view = sg_make_view(tex_view_desc);
    if (sg_query_view_state(tex_view) != SG_RESOURCESTATE_VALID) {
        sg_destroy_view(tex_view);
        sg_destroy_image(image);
        return Err(ImageError::view_failed);
    }

    return Image(Unique<const sg_image>::create(image), nullptr,
                 Unique<const sg_view>::create(tex_view), Vec2(256, 256));
}

Result<Image> Image::create_empty_color(const Vec2 &dims) {
    if (dims.x <= 0 || dims.y <= 0)
        return Err(ImageError::invalid_dimensions);

    sg_image_desc image_desc = {};
    image_desc.usage.color_attachment = true;
    image_desc.width = dims.x;
    image_desc.height = dims.y;
    image_desc.pixel_format = sglue_environment().defaults.color_format;
    image_desc.sample_count = 1;
    image_desc.label = 0;
    const sg_image image = sg_make_image(image_desc);
    if (sg_query_image_state(image) != SG_RESOURCESTATE_VALID) {
        sg_destroy_image(image);
        return Err(ImageError::image_failed);
    }

    sg_view_desc att_view_desc = {};
    att_view_desc.color_attachment.image = image;
    const sg_view att_view = sg_make_view(att_view_desc);
    if (sg_query_view_state(att_view) != SG_RESOURCESTATE_VALID) {
        sg_destroy_image(image);
        sg_destroy_view(att_view);
        return Err(ImageError::view_failed);
    }

    sg_view_desc tex_view_desc = {};
    tex_view_desc.texture.image = image;
    const sg_view tex_view = sg_make_view(tex_view_desc);
    if (sg_query_view_state(tex_view) != SG_RESOURCESTATE_VALID) {
        sg_destroy_image(image);
        sg_destroy_view(att_view);
        sg_destroy_view(tex_view);
        return Err(ImageError::view_failed);
    }

    return Image(Unique<const sg_image>::create(image), Unique<const sg_view>::create(att_view),
                 Unique<const sg_view>::create(tex_view), dims);
}

Result<Image> Image::create_empty_depth(const Vec2 &dims) {
    if (dims.x <= 0 || dims.y <= 0)
        return Err(ImageError::invalid_dimensions);

    sg_image_desc image_desc = {};
    image_desc.usage.depth_stencil_attachment = true;
    image_desc.width = dims.x;
    image_desc.height = dims.y;
    image_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
    image_desc.sample_count = 1;
    image_desc.label = 0;
    const sg_image image = sg_make_image(image_desc);
    if (sg_query_image_state(image) != SG_RESOURCESTATE_VALID) {
        sg_destroy_image(image);
        return Err(ImageError::image_failed);
    }

    sg_view_desc att_view_desc = {};
    att_view_desc.depth_stencil_attachment.image = image;
    const sg_view att_view = sg_make_view(att_view_desc);
    if (sg_query_view_state(att_view) != SG_RESOURCESTATE_VALID) {
        sg_destroy_image(image);
        sg_destroy_view(att_view);
        return Err(ImageError::view_failed);
    }

    sg_view_desc tex_view_desc = {};
    tex_view_desc.texture.image = image;
    const sg_view tex_view = sg_make_view(tex_view_desc);
    if (sg_query_view_state(tex_view) != SG_RESOURCESTATE_VALID) {
        sg_destroy_image(image);
        sg_destroy_view(att_view);
        sg_destroy_view(tex_view);
        return Err(ImageError::view_failed);
    }

    return Image(Unique<const sg_image>::create(image), Unique<const sg_view>::create(att_view),
                 Unique<const sg_view>::create(tex_view), dims);
}

Result<_ProgramLayout>
_ProgramLayout::create(_attrSlotEnumFn attr_slot_enum_fn, _texSlotEnumFn tex_slot_enum_fn,
                       _smpSlotEnumFn smp_slot_enum_fn, _ubSlotEnumFn ub_slot_enum_fn,
                       _ubSizeEnumFn ub_size_enum_fn, _uOffsetEnumFn u_offset_enum_fn,
                       _uDescEnumFn u_desc_enum_fn) {
    Map<String, _attrDesc> attr_map;
    for (auto &[key, val] : attr_slot_enum_fn()) {
        attr_map[key] = _attrDesc{
            .slot = val,
        };
    }
    Map<String, _texDesc> tex_map;
    for (auto &[key, val] : tex_slot_enum_fn()) {
        tex_map[key] = _texDesc{
            .slot = val,
        };
    }
    Map<String, _smpDesc> smp_map;
    for (auto &[key, val] : smp_slot_enum_fn()) {
        smp_map[key] = _smpDesc{
            .slot = val,
        };
    }
    Map<String, _ubDesc> ub_map;
    for (auto &[key, val] : ub_slot_enum_fn()) {
        ub_map[key] = _ubDesc{
            .slot = val,
            .size = SIZE_MAX,
            .members = {},
        };
    }
    for (auto &[key, val] : ub_size_enum_fn()) {
        if (!ub_map.contains(key))
            return Err(PipelineError::bad_layout_definition);
        ub_map[key].size = val;
    }
    for (auto &[key, val] : ub_map) {
        for (auto &[_key, _val] : u_offset_enum_fn(key.c_str())) {
            val.members[_key] = _uDesc{
                .offset = _val,
            };
        }
        for (auto &[_key, _val] : u_desc_enum_fn(key.c_str())) {
            if (!val.members.contains(_key))
                return Err(PipelineError::bad_layout_definition);
            val.members[_key].type = _val.type;
        }
        if (val.members.size() == 0)
            return Err(PipelineError::bad_layout_definition);
    }

    return _ProgramLayout(attr_map, tex_map, smp_map, ub_map);
}

Result<Pipeline> Pipeline::create(_attrSlotEnumFn attr_slot_enum_fn,
                                  _texSlotEnumFn tex_slot_enum_fn, _smpSlotEnumFn smp_slot_enum_fn,
                                  _ubSlotEnumFn ub_slot_enum_fn, _ubSizeEnumFn ub_size_enum_fn,
                                  _uOffsetEnumFn u_offset_enum_fn, _uDescEnumFn u_desc_enum_fn,
                                  const sg_shader_desc *shader_desc) {
    if (!shader_desc)
        return Err(PipelineError::bad_shader_desc);

    const sg_shader shader = sg_make_shader(shader_desc);
    if (sg_query_shader_state(shader) != SG_RESOURCESTATE_VALID)
        return Err(PipelineError::shader_failed);

    sg_pipeline_desc pipeline_desc = {};
    pipeline_desc.shader = shader;
    pipeline_desc.colors[0] = {};
    pipeline_desc.index_type = SG_INDEXTYPE_UINT32;
    Vertex::fill_layout_attrs(pipeline_desc.layout);
    const sg_pipeline pipeline = sg_make_pipeline(pipeline_desc);
    if (sg_query_pipeline_state(pipeline) != SG_RESOURCESTATE_VALID) {
        sg_destroy_pipeline(pipeline);
        return Err(PipelineError::pipeline_failed);
    }

    _ProgramLayout program_layout = TRY(
        _ProgramLayout::create(attr_slot_enum_fn, tex_slot_enum_fn, smp_slot_enum_fn,
                               ub_slot_enum_fn, ub_size_enum_fn, u_offset_enum_fn, u_desc_enum_fn));

    return Pipeline(Unique<const sg_pipeline>::create(pipeline), program_layout);
}

Result<Sampler> Sampler::create(Filter filter, Border border, Wrap wrap) {
    sg_sampler_desc sampler_desc = {};
    sampler_desc.mag_filter = filter == Filter::LINEAR ? SG_FILTER_LINEAR : SG_FILTER_NEAREST;
    sampler_desc.min_filter = sampler_desc.mag_filter;
    sampler_desc.mipmap_filter = sampler_desc.mag_filter;
    if (border == Border::BLACK)
        sampler_desc.border_color = SG_BORDERCOLOR_OPAQUE_BLACK;
    else if (border == Border::WHITE)
        sampler_desc.border_color = SG_BORDERCOLOR_OPAQUE_WHITE;
    else
        sampler_desc.border_color = SG_BORDERCOLOR_TRANSPARENT_BLACK;
    sampler_desc.wrap_u = wrap == Wrap::CLAMP ? SG_WRAP_CLAMP_TO_BORDER : SG_WRAP_REPEAT;
    sampler_desc.wrap_v = wrap == Wrap::CLAMP ? SG_WRAP_CLAMP_TO_BORDER : SG_WRAP_REPEAT;
    const sg_sampler sampler = sg_make_sampler(sampler_desc);
    if (sg_query_sampler_state(sampler) != SG_RESOURCESTATE_VALID) {
        sg_destroy_sampler(sampler);
        return Err(SamplerError::sampler_failed);
    }

    return Sampler(Unique<const sg_sampler>::create(sampler));
}

Result<Mesh> Mesh::create_quad(Vec3 p, Vec2 s, bool colored) {
    f32 nc = colored ? 0.0f : 1.0f;
    std::array<Vertex, 4> vertices = {
        Vertex{                    p, {0, 0, -1},  {1.f, nc, nc, 1.f}, {0.f, 0.f}},
        Vertex{  p + Vec3{0, s.y, 0}, {0, 0, -1},  {nc, 1.f, nc, 1.f}, {0.f, 1.f}},
        Vertex{  p + Vec3{s.x, 0, 0}, {0, 0, -1},  {nc, nc, 1.f, 1.f}, {1.f, 0.f}},
        Vertex{p + Vec3{s.x, s.y, 0}, {0, 0, -1}, {1.f, 1.f, nc, 1.f}, {1.f, 1.f}},
    };

    std::array<u32, 6> indices = {
        0, 2, 1, 1, 3, 2,
    };

    sg_buffer_desc vertex_buffer_desc = {};
    vertex_buffer_desc.data =
        sg_range{.ptr = vertices.data(), .size = sizeof(Vertex) * vertices.size()};
    vertex_buffer_desc.usage.vertex_buffer = true;

    const sg_buffer vertex_buffer = sg_make_buffer(vertex_buffer_desc);
    if (sg_query_buffer_state(vertex_buffer) != SG_RESOURCESTATE_VALID) {
        sg_destroy_buffer(vertex_buffer);
        return Err(MeshError::vertex_buffer_failed);
    }

    sg_buffer_desc index_buffer_desc = {};
    index_buffer_desc.data = sg_range{.ptr = indices.data(), .size = 4 * indices.size()};
    index_buffer_desc.usage.index_buffer = true;

    const sg_buffer index_buffer = sg_make_buffer(index_buffer_desc);
    if (sg_query_buffer_state(index_buffer) != SG_RESOURCESTATE_VALID) {
        sg_destroy_buffer(vertex_buffer);
        sg_destroy_buffer(index_buffer);
        return Err(MeshError::index_buffer_failed);
    }

    return Mesh(Unique<const sg_buffer>::create(vertex_buffer),
                Unique<const sg_buffer>::create(index_buffer), indices.size());
}

Result<Framebuffer> Framebuffer::create(const Vec2 &dims) {
    if (dims.x < 0 || dims.y < 0)
        return Err(FramebufferError::invalid_dimensions);

    auto color = TRY(Image::create_empty_color(dims));

    auto depth = TRY(Image::create_empty_depth(dims));

    return Framebuffer(Unique<Image>::create(std::move(color)),
                       Unique<Image>::create(std::move(depth)), dims);
}
} // namespace render
