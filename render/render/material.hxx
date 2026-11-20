/*
 *  render/material.hxx
 *
 *  individual uniforms over shared pipelines
 */

#ifndef simulator_render_material_hxx
#define simulator_render_material_hxx

#include <algorithm>
#include <core/core.hxx>
#include <iterator>

#include "sokol_gfx.h"

#include "image.hxx"
#include "pipeline.hxx"
#include "sampler.hxx"

// TODO: Global uniforms

namespace render {
using namespace core;

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
    static _Uniforms create(const _ProgramLayout &program_layout) {
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
        std::sort(ub_vec.begin(), ub_vec.end(),
                  [](_ubDesc a, _ubDesc b) { return a.slot < b.slot; });
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
    static Option<Material> create(Shared<Pipeline> pipeline) {
        if (!pipeline)
            return {};
        const _ProgramLayout &layout = pipeline->_get_layout();

        _Uniforms uniforms = _Uniforms::create(layout);

        return Material(pipeline, uniforms);
    }

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
