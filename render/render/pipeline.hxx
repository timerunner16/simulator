/*
 *  render/pipeline.hxx
 *
 *  c++ wrapper over sokol pipelines
 */

#ifndef simulator_render_pipeline_hxx
#define simulator_render_pipeline_hxx

#include <core/core.hxx>

#include "sokol_gfx.h"

#include "mesh.hxx"

namespace render {
using namespace core;

class Pipeline {
  private:
    sg_pipeline m_pipeline;

    explicit Pipeline(sg_pipeline pipeline) : m_pipeline(pipeline) {}

  public:
    Pipeline(const Pipeline &other) = delete;
    Pipeline &operator=(const Pipeline &other) = delete;

    Pipeline(Pipeline &&other) : m_pipeline(other.m_pipeline) {
        other.m_pipeline = {};
    }

    Pipeline &operator=(Pipeline &&other) {
        m_pipeline = other.m_pipeline;
        other.m_pipeline = {};
        return *this;
    }

    static Option<Pipeline> create_from_shaderdesc(const sg_shader_desc &shader_desc) {
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

        return Pipeline(pipeline);
    }

    const sg_pipeline _get_pipeline() {
        return m_pipeline;
    }
};
} // namespace render

#endif
