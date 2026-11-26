/*
 *  render/pipeline.hxx
 *
 *  c++ wrapper over sokol pipelines
 */

#ifndef simulator_render_pipeline_hxx
#define simulator_render_pipeline_hxx

#include <core/core.hxx>

#include <map>

#include "sokol_gfx.h"

template <> struct std::default_delete<const sg_pipeline> {
    constexpr default_delete() = default;

    void operator()(const sg_pipeline *pipeline) const noexcept {
        if (pipeline) {
            sg_destroy_pipeline(*pipeline);
        }
    }
};

namespace render {
using namespace core;

namespace util {
template <typename T> struct sg_uniform_type_from {
    static constexpr sg_uniform_type value = SG_UNIFORMTYPE_INVALID;
};

template <> struct sg_uniform_type_from<i32> {
    static constexpr sg_uniform_type value = SG_UNIFORMTYPE_INT;
};

template <> struct sg_uniform_type_from<f32> {
    static constexpr sg_uniform_type value = SG_UNIFORMTYPE_FLOAT;
};

template <> struct sg_uniform_type_from<Vec2> {
    static constexpr sg_uniform_type value = SG_UNIFORMTYPE_FLOAT2;
};

template <> struct sg_uniform_type_from<Vec3> {
    static constexpr sg_uniform_type value = SG_UNIFORMTYPE_FLOAT3;
};

template <> struct sg_uniform_type_from<Vec4> {
    static constexpr sg_uniform_type value = SG_UNIFORMTYPE_FLOAT4;
};

template <> struct sg_uniform_type_from<Mat4> {
    static constexpr sg_uniform_type value = SG_UNIFORMTYPE_MAT4;
};

template <typename T>
static constexpr sg_uniform_type sg_uniform_type_from_v = sg_uniform_type_from<T>::value;

template <typename T>
static constexpr bool is_uniform_type_v = sg_uniform_type_from_v<T> != SG_UNIFORMTYPE_INVALID;
} // namespace util

using _attrSlotEnumFn = Fn<Span<Pair<const char *, int>>>;
using _texSlotEnumFn = Fn<Span<Pair<const char *, int>>>;
using _smpSlotEnumFn = Fn<Span<Pair<const char *, int>>>;
using _ubSlotEnumFn = Fn<Span<Pair<const char *, int>>>;
using _ubSizeEnumFn = Fn<Span<Pair<const char *, size_t>>>;
using _uOffsetEnumFn = Fn<Span<Pair<const char *, int>>, const char *>;
using _uDescEnumFn = Fn<Span<Pair<const char *, sg_glsl_shader_uniform>>, const char *>;

struct _attrDesc {
    i32 slot;
};

struct _texDesc {
    i32 slot;
};

struct _smpDesc {
    i32 slot;
};

struct _uDesc {
    i32 offset;
    sg_uniform_type type;
};

struct _ubDesc {
    i32 slot;
    size_t size;
    Map<String, _uDesc> members;
};

#define PIPELINE_ERROR(X)                                                                          \
    X(bad_layout_definition, 1, "bad layout definition")                                           \
    X(bad_shader_desc, 2, "bad shader description")                                                \
    X(shader_failed, 3, "shader failed to construct")                                              \
    X(pipeline_failed, 4, "pipeline failed to construct")

DEFINE_ERRCODE(PipelineError, PIPELINE_ERROR, 1, render)
#undef PIPELINE_ERROR

class _ProgramLayout {
  private:
    Map<String, _attrDesc> m_attr_map;
    Map<String, _texDesc> m_tex_map;
    Map<String, _smpDesc> m_smp_map;
    Map<String, _ubDesc> m_ub_map;

    explicit _ProgramLayout(Map<String, _attrDesc> attr_map, Map<String, _texDesc> tex_map,
                            Map<String, _smpDesc> smp_map, Map<String, _ubDesc> ub_map)
        : m_attr_map(attr_map), m_tex_map(tex_map), m_smp_map(smp_map), m_ub_map(ub_map) {}

  public:
    static Result<_ProgramLayout>
    create(_attrSlotEnumFn attr_slot_enum_fn, _texSlotEnumFn tex_slot_enum_fn,
           _smpSlotEnumFn smp_slot_enum_fn, _ubSlotEnumFn ub_slot_enum_fn,
           _ubSizeEnumFn ub_size_enum_fn, _uOffsetEnumFn u_offset_enum_fn,
           _uDescEnumFn u_desc_enum_fn);

    const Map<String, _attrDesc> &get_attr_map() const {
        return m_attr_map;
    }

    const Map<String, _texDesc> &get_tex_map() const {
        return m_tex_map;
    }

    const Map<String, _smpDesc> &get_smp_map() const {
        return m_smp_map;
    }

    const Map<String, _ubDesc> &get_ub_map() const {
        return m_ub_map;
    }

    template <typename T, typename = std::enable_if<util::is_uniform_type_v<T>>>
    Option<Pair<u32, usize>> find_uniform(String ub_name, String u_name) const {
        if (!m_ub_map.contains(ub_name))
            return {};
        _ubDesc ub_desc = m_ub_map.at(ub_name);
        if (!ub_desc.members.contains(u_name))
            return {};
        _uDesc u_desc = ub_desc.members.at(u_name);
        if (u_desc.type != util::sg_uniform_type_from_v<T>)
            return {};
        Pair<u32, usize> location = {ub_desc.slot, u_desc.offset};
        return location;
    }

    Option<usize> find_texture(String tex_name) const {
        if (!m_tex_map.contains(tex_name))
            return {};
        return m_tex_map.at(tex_name).slot;
    }

    Option<usize> find_sampler(String smp_name) const {
        if (!m_smp_map.contains(smp_name))
            return {};
        return m_smp_map.at(smp_name).slot;
    }
};

class Pipeline {
  private:
    Unique<const sg_pipeline> m_pipeline;

    const _ProgramLayout m_program_layout;

    explicit Pipeline(Unique<const sg_pipeline> &&pipeline, const _ProgramLayout program_layout)
        : m_pipeline(std::move(pipeline)), m_program_layout(program_layout) {}

  public:
    static Result<Pipeline> create(_attrSlotEnumFn attr_slot_enum_fn,
                                   _texSlotEnumFn tex_slot_enum_fn, _smpSlotEnumFn smp_slot_enum_fn,
                                   _ubSlotEnumFn ub_slot_enum_fn, _ubSizeEnumFn ub_size_enum_fn,
                                   _uOffsetEnumFn u_offset_enum_fn, _uDescEnumFn u_desc_enum_fn,
                                   const sg_shader_desc *shader_desc);

    const _ProgramLayout &_get_layout() const {
        return m_program_layout;
    }

    void _apply_pipeline() const {
        sg_apply_pipeline(*m_pipeline);
    }
};

#define PIPELINE_CREATE_ARGS(mod, prog, backend)                                                   \
    mod##_##prog##_enum_attr_slot, mod##_##prog##_enum_texture_slot,                               \
        mod##_##prog##_enum_sampler_slot, mod##_##prog##_enum_uniformblock_slot,                   \
        mod##_##prog##_enum_uniformblock_size, mod##_##prog##_enum_uniform_offset,                 \
        mod##_##prog##_enum_uniform_desc, mod##_##prog##_shader_desc((backend))

#define PIPELINE_CREATE_ARGS_NOMOD(prog, backend)                                                  \
    prog##_enum_attr_slot, prog##_enum_texture_slot, prog##_enum_sampler_slot,                     \
        prog##_enum_uniformblock_slot, prog##_enum_uniformblock_size, prog##_enum_uniform_offset,  \
        prog##_enum_uniform_desc, prog##_shader_desc((backend))
} // namespace render

#endif
