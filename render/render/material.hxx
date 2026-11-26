/*
 *  render/material.hxx
 *
 *  individual uniforms over shared pipelines
 */

#ifndef simulator_render_material_hxx
#define simulator_render_material_hxx

#include <core/core.hxx>

#include "sokol_gfx.h"

#include "image.hxx"
#include "pipeline.hxx"
#include "sampler.hxx"

#include <cstring>

// TODO: Global uniforms

namespace render {
using namespace core;

#define MATERIAL_ERROR(X) X(bad_pipeline, 1, "bad pipeline")

DEFINE_ERRCODE(MaterialError, MATERIAL_ERROR, 1, render)
#undef MATERIAL_ERROR

String get_error_string(const MaterialError error);

class _Uniforms {
  private:
    Vector<Shared<Image>> m_tex_refs;
    Vector<Shared<Sampler>> m_smp_refs;

    Vector<u8> m_ub_data;
    Vector<usize> m_ub_offsets;
    Vector<usize> m_ub_sizes;

    const _ProgramLayout &m_program_layout;

    explicit _Uniforms(Vector<Shared<Image>> tex_refs, Vector<Shared<Sampler>> smp_refs,
                       Vector<u8> ub_data, Vector<usize> ub_offsets, Vector<usize> ub_sizes,
                       const _ProgramLayout &program_layout)
        : m_tex_refs(tex_refs), m_smp_refs(smp_refs), m_ub_data(ub_data), m_ub_offsets(ub_offsets),
          m_ub_sizes(ub_sizes), m_program_layout(program_layout) {}

  public:
    static _Uniforms create(const _ProgramLayout &program_layout);

    template <typename T, typename = std::enable_if<util::is_uniform_type_v<T>>>
    void set_uniform(T data, String ub_name, String u_name) {
        if (Option<Pair<u32, usize>> location = m_program_layout.find_uniform<T>(ub_name, u_name)) {
            usize offset = m_ub_offsets[location->first] + location->second;

            Array<u8, sizeof(T)> data_bytes = std::bit_cast<Array<u8, sizeof(T)>>(data);
            std::memcpy(m_ub_data.data() + offset, &data_bytes, sizeof(T));
        }
    }

    void set_texture(Shared<Image> texture, String tex_name) {
        if (Option<usize> slot = m_program_layout.find_texture(tex_name)) {
            m_tex_refs[*slot] = texture;
        }
    }

    void set_sampler(Shared<Sampler> sampler, String smp_name) {
        if (Option<usize> slot = m_program_layout.find_sampler(smp_name)) {
            m_smp_refs[*slot] = sampler;
        }
    }

    void apply_uniforms() {
        for (u32 ub_slot = 0; ub_slot < m_ub_offsets.size(); ub_slot++) {
            usize offset = m_ub_offsets[ub_slot];
            usize size = m_ub_sizes[ub_slot];
            sg_range range = sg_range{m_ub_data.data() + offset, size};
            sg_apply_uniforms(ub_slot, range);
        }
    }

    sg_bindings get_bindings() {
        sg_bindings bindings = {};
        for (u32 tex_slot = 0; tex_slot < m_tex_refs.size(); tex_slot++) {
            Shared<Image> tex = m_tex_refs[tex_slot];
            if (tex)
                bindings.views[tex_slot] = tex->_get_tex_view();
            else
                bindings.views[tex_slot] = {};
        }
        for (u32 smp_slot = 0; smp_slot < m_smp_refs.size(); smp_slot++) {
            Shared<Sampler> smp = m_smp_refs[smp_slot];
            if (smp)
                bindings.samplers[smp_slot] = smp->_get_sampler();
            else
                bindings.samplers[smp_slot] = {};
        }
        return bindings;
    }
};

class Material {
  private:
    Shared<Pipeline> m_pipeline;
    _Uniforms m_uniforms;

    explicit Material(Shared<Pipeline> pipeline, _Uniforms uniforms)
        : m_pipeline(pipeline), m_uniforms(uniforms) {}

  public:
    static Result<Material> create(Shared<Pipeline> pipeline);

    template <typename T, typename = std::enable_if<util::is_uniform_type_v<T>>>
    void set_uniform(T data, String ub_name, String u_name) {
        m_uniforms.set_uniform(data, ub_name, u_name);
    }

    void set_texture(Shared<Image> texture, String tex_name) {
        m_uniforms.set_texture(texture, tex_name);
    }

    void set_sampler(Shared<Sampler> sampler, String smp_name) {
        m_uniforms.set_sampler(sampler, smp_name);
    }

    void _apply_uniforms() {
        m_uniforms.apply_uniforms();
    }

    sg_bindings _get_bindings() {
        return m_uniforms.get_bindings();
    }

    const Pipeline &_get_pipeline() {
        return *m_pipeline.get();
    }
};
} // namespace render

#endif
