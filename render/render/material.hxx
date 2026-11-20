/*
 *  render/material.hxx
 *
 *  individual uniforms over shared pipelines
 */

#ifndef simulator_render_material_hxx
#define simulator_render_material_hxx

#include <core/core.hxx>

#include "sokol_gfx.h"

#include "pipeline.hxx"

// TODO: Shared program descriptor
// TODO: Hold uniforms in material based on descriptor
// TODO: Instance vs base uniforms

namespace render {
using namespace core;

class _Uniforms {
  private:
    Array<Shared<Image>, 8> m_tex_refs;
    Array<Shared<Sampler>, 8> m_smp_refs;
    Array<Unique<u8[]>, 8> m_ub_data;

    explicit _Uniforms(Array<Shared<Image>, 8> &&tex_refs, Array<Shared<Sampler>, 8> &&smp_refs,
                       Array<Unique<u8[]>, 8> &&ub_data)
        : m_tex_refs(std::move(tex_refs)), m_smp_refs(std::move(smp_refs)),
          m_ub_data(std::move(ub_data)) {}

  public:
    static Option<_Uniforms> create(const _ProgramLayout &program_layout) {
        auto &ub_map = program_layout.get_ub_map();
        for (auto &[ub_name, ub_desc] : ub_map) {
            m_ub_data[ub_desc.slot] = std::make_unique(new u8[ub_desc.size]);
        }

        return {};
    }
};

class Material {
  private:
    Shared<Pipeline> m_pipeline;
    _Uniforms m_uniforms;

    explicit Material(Shared<Pipeline> pipeline, _Uniforms&& uniforms)
        : m_pipeline(pipeline), m_uniforms(std::move(uniforms)) {}

  public:
};
} // namespace render

#endif
