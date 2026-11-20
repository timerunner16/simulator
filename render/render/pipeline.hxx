/*
 *  render/pipeline.hxx
 *
 *  c++ wrapper over sokol pipelines
 */

#ifndef simulator_render_pipeline_hxx
#define simulator_render_pipeline_hxx

#include <array>
#include <core/core.hxx>
#include <cstring>
#include <map>

#include "sokol_gfx.h"

#include "mesh.hxx"

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
    static Option<_ProgramLayout>
    create(_attrSlotEnumFn attr_slot_enum_fn, _texSlotEnumFn tex_slot_enum_fn,
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
                return {};
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
                    return {};
                val.members[_key].type = _val.type;
            }
            if (val.members.size() == 0)
                return {};
        }

        return _ProgramLayout(attr_map, tex_map, smp_map, ub_map);
    }

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
    sg_pipeline m_pipeline;

    _ProgramLayout m_program_layout;

    explicit Pipeline(sg_pipeline pipeline, const _ProgramLayout program_layout)
        : m_pipeline(pipeline), m_program_layout(program_layout) {}

  public:
    Pipeline(const Pipeline &other) = delete;
    Pipeline &operator=(const Pipeline &other) = delete;

    Pipeline(Pipeline &&other)
        : m_pipeline(other.m_pipeline), m_program_layout(other.m_program_layout) {
        other.m_pipeline = {};
    }

    Pipeline &operator=(Pipeline &&other) {
        m_pipeline = other.m_pipeline;
        m_program_layout = other.m_program_layout;
        other.m_pipeline = {};
        return *this;
    }

    static Option<Pipeline> create(_attrSlotEnumFn attr_slot_enum_fn,
                                   _texSlotEnumFn tex_slot_enum_fn, _smpSlotEnumFn smp_slot_enum_fn,
                                   _ubSlotEnumFn ub_slot_enum_fn, _ubSizeEnumFn ub_size_enum_fn,
                                   _uOffsetEnumFn u_offset_enum_fn, _uDescEnumFn u_desc_enum_fn,
                                   const sg_shader_desc *shader_desc) {
        if (!shader_desc)
            return {};

        const sg_shader shader = sg_make_shader(shader_desc);
        if (sg_query_shader_state(shader) != SG_RESOURCESTATE_VALID)
            return {};

        sg_pipeline_desc pipeline_desc = {};
        pipeline_desc.shader = shader;
        pipeline_desc.index_type = SG_INDEXTYPE_UINT32;
        Vertex::fill_layout_attrs(pipeline_desc.layout);
        const sg_pipeline pipeline = sg_make_pipeline(pipeline_desc);
        if (sg_query_pipeline_state(pipeline) != SG_RESOURCESTATE_VALID)
            return {};

        Option<_ProgramLayout> program_layout = _ProgramLayout::create(
            attr_slot_enum_fn, tex_slot_enum_fn, smp_slot_enum_fn, ub_slot_enum_fn, ub_size_enum_fn,
            u_offset_enum_fn, u_desc_enum_fn);
        if (!program_layout)
            return {};

        return Pipeline(pipeline, *program_layout);
    }

    const _ProgramLayout &_get_layout() const {
        return m_program_layout;
    }

    void _apply_pipeline() const {
        sg_apply_pipeline(m_pipeline);
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

// #define PIPELINE_CREATE_ARGS(mod, prog, backend) \
//     mod##_##prog##_uniformblock_slot, mod##_##prog##_uniformblock_size, \
//         mod##_##prog##_uniform_offset, mod##_##prog##_uniform_desc, mod##_##prog##_texture_slot,
//         \ mod##_##prog##_sampler_slot, mod##_##prog##_shader_desc((backend))
//
// #define PIPELINE_CREATE_ARGS_NOMOD(prog, backend) \
//     prog##_uniformblock_slot, prog##_uniformblock_size, prog##_uniform_offset, \
//         prog##_uniform_desc, prog##_texture_slot, prog##_sampler_slot, \
//         &prog##_shader_desc((backend))
} // namespace render

#endif
