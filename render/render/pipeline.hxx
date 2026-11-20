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

#include <string>
#include <string_view>

#include "sokol_gfx.h"

#include "image.hxx"
#include "mesh.hxx"
#include "sampler.hxx"

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

class _ProgramLayout {
  private:
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
                .size = UINT64_MAX,
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

    const Map<String, _attrDesc>& get_attr_map() const {
        return m_attr_map;
    }

    const Map<String, _texDesc>& get_tex_map() const {
        return m_tex_map;
    }

    const Map<String, _smpDesc>& get_smp_map() const {
        return m_smp_map;
    }

    const Map<String, _ubDesc>& get_ub_map() const {
        return m_ub_map;
    }
};

using _ubSlotFn = Fn<int, const char *>;
using _ubSizeFn = Fn<size_t, const char *>;
using _uOffsetFn = Fn<int, const char *, const char *>;
using _uGlslFn = Fn<sg_glsl_shader_uniform, const char *, const char *>;
using _texSlotFn = Fn<int, const char *>;
using _smpSlotFn = Fn<int, const char *>;

class _ProgramUniforms {
  private:
    struct _uDesc {
        i32 offset;
        sg_uniform_type type;
    };

    struct _ubDesc {
        i32 slot;
        usize size;
        std::map<std::string_view, _uDesc> offset_map;
    };

    std::map<std::string_view, _ubDesc> m_ub_map;

    struct _texDesc {
        i32 slot;
    };

    struct _smpDesc {
        i32 slot;
    };

    std::map<std::string_view, _texDesc> m_tex_map;
    std::map<std::string_view, _smpDesc> m_smp_map;
    std::array<OptionRef<const Image>, 8> m_tex_refs;
    std::array<OptionRef<const Sampler>, 8> m_smp_refs;

    _ubSlotFn m_ub_slot_fn;
    _ubSizeFn m_ub_size_fn;
    _uOffsetFn m_u_offset_fn;
    _uGlslFn m_u_glsl_fn;
    _texSlotFn m_tex_slot_fn;
    _smpSlotFn m_smp_slot_fn;

    std::array<Unique<u8[]>, 8> m_ub_data;

    bool map_uniform_block(std::string_view ub_name) {
        std::string ub_name_nt(ub_name);

        i32 slot = m_ub_slot_fn(ub_name_nt.c_str());
        usize size = m_ub_size_fn(ub_name_nt.c_str());
        if (slot == -1 || size == 0)
            return false;

        m_ub_map[ub_name] = _ubDesc{.slot = slot, .size = size};

        m_ub_data[slot] = std::make_unique<u8[]>(size);

        return true;
    }

    bool map_uniform(_ubDesc &ub_desc, std::string_view ub_name, std::string_view u_name) {
        std::string ub_name_nt(ub_name);
        std::string u_name_nt(u_name);

        i32 slot = m_ub_slot_fn(ub_name_nt.c_str());
        usize size = m_ub_size_fn(ub_name_nt.c_str());
        i32 offset = m_u_offset_fn(ub_name_nt.c_str(), u_name_nt.c_str());
        sg_glsl_shader_uniform uniform = m_u_glsl_fn(ub_name_nt.c_str(), u_name_nt.c_str());

        if (slot == -1 || size == 0 || offset == -1)
            return false;
        ub_desc.offset_map[u_name] = _uDesc{
            .offset = offset,
            .type = uniform.type,
        };

        return true;
    }

    bool map_texture(std::string_view tex_name) {
        std::string tex_name_nt(tex_name);

        i32 slot = m_tex_slot_fn(tex_name_nt.c_str());

        if (slot == -1)
            return false;

        m_tex_map[tex_name] = _texDesc{.slot = slot};

        return true;
    }

    bool map_sampler(std::string_view smp_name) {
        std::string smp_name_nt(smp_name);

        i32 slot = m_smp_slot_fn(smp_name_nt.c_str());

        if (slot == -1)
            return false;

        m_smp_map[smp_name] = _smpDesc{.slot = slot};

        return true;
    }

  public:
    explicit _ProgramUniforms(_ubSlotFn ub_slot_fn, _ubSizeFn ub_size_fn, _uOffsetFn u_offset_fn,
                              _uGlslFn u_glsl_fn, _texSlotFn tex_slot_fn, _smpSlotFn smp_slot_fn)
        : m_ub_slot_fn(ub_slot_fn), m_ub_size_fn(ub_size_fn), m_u_offset_fn(u_offset_fn),
          m_u_glsl_fn(u_glsl_fn), m_tex_slot_fn(tex_slot_fn), m_smp_slot_fn(smp_slot_fn) {}

    template <typename T, typename = std::enable_if<util::is_uniform_type_v<T>>>
    void set_uniform(T data, std::string_view ub_name, std::string_view u_name) {
        if (!m_ub_map.contains(ub_name))
            if (!map_uniform_block(ub_name))
                return;

        _ubDesc &ub_desc = m_ub_map[ub_name];

        if (!ub_desc.offset_map.contains(u_name))
            if (!map_uniform(ub_desc, ub_name, u_name))
                return;

        _uDesc &u_desc = ub_desc.offset_map[u_name];

        if (util::sg_uniform_type_from_v<T> != u_desc.type)
            return;

        // - convert data to bytes,
        // - copy bytes into uniform block at offset
        // no type punning --> no UB :]
        auto data_bytes = std::bit_cast<std::array<u8, sizeof(T)>>(data);
        auto &ub = m_ub_data[ub_desc.slot];
        std::memcpy(ub.get() + u_desc.offset, &data_bytes, sizeof(T));
    }

    void set_texture(const Image &image, std::string_view tex_name) {
        if (!m_tex_map.contains(tex_name))
            if (!map_texture(tex_name))
                return;

        _texDesc &tex_desc = m_tex_map[tex_name];

        m_tex_refs[tex_desc.slot] = image;
    }

    void set_sampler(const Sampler &sampler, std::string_view smp_name) {
        if (!m_smp_map.contains(smp_name))
            if (!map_sampler(smp_name))
                return;

        _smpDesc &smp_desc = m_smp_map[smp_name];

        m_smp_refs[smp_desc.slot] = sampler;
    }

    void apply_uniforms() {
        for (auto &[_, ub] : m_ub_map) {
            sg_range range = sg_range{.ptr = m_ub_data[ub.slot].get(), .size = ub.size};
            sg_apply_uniforms(ub.slot, range);
        }
    }

    sg_bindings get_texsmp_bindings() {
        sg_bindings bindings = {};
        for (auto &[_, tex] : m_tex_map) {
            if (m_tex_refs[tex.slot].has_value()) {
                const sg_view &view = m_tex_refs[tex.slot]->get()._get_tex_view();
                bindings.views[tex.slot] = view;
            } else {
                bindings.views[tex.slot] = {};
            }
        }
        for (auto &[_, smp] : m_smp_map) {
            if (m_smp_refs[smp.slot].has_value()) {
                const sg_sampler &sampler = m_smp_refs[smp.slot]->get()._get_sampler();
                bindings.samplers[smp.slot] = sampler;
            } else {
                bindings.samplers[smp.slot] = {};
            }
        }
        return bindings;
    };
};

class Pipeline {
  private:
    sg_pipeline m_pipeline;

    _ProgramUniforms m_program_uniforms;

    explicit Pipeline(sg_pipeline pipeline, _ProgramUniforms program_uniforms)
        : m_pipeline(pipeline), m_program_uniforms(std::move(program_uniforms)) {}

  public:
    Pipeline(const Pipeline &other) = delete;
    Pipeline &operator=(const Pipeline &other) = delete;

    Pipeline(Pipeline &&other)
        : m_pipeline(other.m_pipeline), m_program_uniforms(std::move(other.m_program_uniforms)) {
        other.m_pipeline = {};
    }

    Pipeline &operator=(Pipeline &&other) {
        m_pipeline = other.m_pipeline;
        m_program_uniforms = std::move(other.m_program_uniforms);
        other.m_pipeline = {};
        return *this;
    }

    static Option<Pipeline> create(_ubSlotFn ub_slot_fn, _ubSizeFn ub_size_fn,
                                   _uOffsetFn u_offset_fn, _uGlslFn u_glsl_fn,
                                   _texSlotFn tex_slot_fn, _smpSlotFn smp_slot_fn,
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

        _ProgramUniforms program_uniforms(ub_slot_fn, ub_size_fn, u_offset_fn, u_glsl_fn,
                                          tex_slot_fn, smp_slot_fn);

        return Pipeline(pipeline, std::move(program_uniforms));
    }

    template <typename T, typename = std::enable_if<util::is_uniform_type_v<T>>>
    void set_uniform(T data, std::string_view ub_name, std::string_view u_name) {
        m_program_uniforms.set_uniform<T>(data, ub_name, u_name);
    }

    void set_texture(const Image &image, std::string_view tex_name) {
        m_program_uniforms.set_texture(image, tex_name);
    }

    void set_sampler(const Sampler &sampler, std::string_view smp_name) {
        m_program_uniforms.set_sampler(sampler, smp_name);
    }

    void apply_uniforms() {
        m_program_uniforms.apply_uniforms();
    }

    sg_bindings _get_texsmp_bindings() {
        return m_program_uniforms.get_texsmp_bindings();
    }

    void apply_pipeline() {
        sg_apply_pipeline(m_pipeline);
    }
};

#define PIPELINE_CREATE_ARGS(mod, prog, backend)                                                   \
    mod##_##prog##_uniformblock_slot, mod##_##prog##_uniformblock_size,                            \
        mod##_##prog##_uniform_offset, mod##_##prog##_uniform_desc, mod##_##prog##_texture_slot,   \
        mod##_##prog##_sampler_slot, mod##_##prog##_shader_desc((backend))

#define PIPELINE_CREATE_ARGS_NOMOD(prog, backend)                                                  \
    prog##_uniformblock_slot, prog##_uniformblock_size, prog##_uniform_offset,                     \
        prog##_uniform_desc, prog##_texture_slot, prog##_sampler_slot,                             \
        &prog##_shader_desc((backend))
} // namespace render

#endif
