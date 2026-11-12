/*
 *  render/mesh.hxx
 *
 *  mesh storage, loading, management
 */

#ifndef simulator_render_mesh_hxx
#define simulator_render_mesh_hxx

#include <sokol_gfx.h>

#include <core/core.hxx>

namespace render {
using namespace core;

struct Vertex {
    Vec3 position = {0, 0, 0};
    Vec3 normal = {0, 1, 0};
    Vec4 color = {1, 1, 1, 1};
    Vec2 uv = {0, 0};

    static void fill_layout_attrs(sg_vertex_layout_state &layout) {
        layout.buffers[0] = {.stride = 48, .step_func = SG_VERTEXSTEP_PER_VERTEX, .step_rate = 1};
        layout.attrs[0] = {.buffer_index = 0, .offset = 0, .format = SG_VERTEXFORMAT_FLOAT3};
        layout.attrs[1] = {.buffer_index = 0, .offset = 12, .format = SG_VERTEXFORMAT_FLOAT3};
        layout.attrs[2] = {.buffer_index = 0, .offset = 24, .format = SG_VERTEXFORMAT_FLOAT4};
        layout.attrs[3] = {.buffer_index = 0, .offset = 40, .format = SG_VERTEXFORMAT_FLOAT2};
    }
};

class Mesh {
  private:
    sg_buffer m_vertex_buffer;
    sg_buffer m_index_buffer;

    explicit Mesh(sg_buffer vertex_buffer, sg_buffer index_buffer)
        : m_vertex_buffer(vertex_buffer), m_index_buffer(index_buffer) {};

  public:
    Mesh(const Mesh &other) = delete;
    Mesh &operator=(const Mesh &other) = delete;

    Mesh(Mesh &&other)
        : m_vertex_buffer(other.m_vertex_buffer), m_index_buffer(other.m_index_buffer) {
        other.m_vertex_buffer = {};
        other.m_index_buffer = {};
    }

    Mesh &operator=(Mesh &&other) {
        m_vertex_buffer = other.m_vertex_buffer;
        m_index_buffer = other.m_index_buffer;
        other.m_vertex_buffer = {};
        other.m_index_buffer = {};
        return *this;
    }

    ~Mesh() {
        sg_destroy_buffer(m_vertex_buffer);
        sg_destroy_buffer(m_index_buffer);
    }

    static Option<Mesh> create_quad() {
        const Vertex vertices[] = {
            Vertex{{-.5f, -.5f, 0.f}, {0.f, 0.f, -1.f}, {1.f, 0.f, 0.f, 1.f}, {0.f, 0.f}},
            Vertex{ {-.5f, .5f, 0.f}, {0.f, 0.f, -1.f}, {0.f, 1.f, 0.f, 1.f}, {0.f, 1.f}},
            Vertex{ {.5f, -.5f, 0.f}, {0.f, 0.f, -1.f}, {0.f, 0.f, 1.f, 1.f}, {1.f, 0.f}},
            Vertex{  {.5f, .5f, 0.f}, {0.f, 0.f, -1.f}, {1.f, 1.f, 0.f, 1.f}, {1.f, 1.f}},
        };

        const u32 indices[] = {
            0, 2, 1, 1, 3, 2,
        };

        sg_buffer_desc vertex_buffer_desc = {};
        vertex_buffer_desc.data = SG_RANGE(vertices);
        vertex_buffer_desc.usage.vertex_buffer = true;

        const sg_buffer vertex_buffer = sg_make_buffer(vertex_buffer_desc);
        if (sg_query_buffer_state(vertex_buffer) != SG_RESOURCESTATE_VALID)
            return {};

        sg_buffer_desc index_buffer_desc = {};
        index_buffer_desc.data = SG_RANGE(indices);
        index_buffer_desc.usage.index_buffer = true;

        const sg_buffer index_buffer = sg_make_buffer(index_buffer_desc);
        if (sg_query_buffer_state(index_buffer) != SG_RESOURCESTATE_VALID)
            return {};

        return Mesh(vertex_buffer, index_buffer);
    }

    const sg_buffer _get_vertex_buffer() const {
        return m_vertex_buffer;
    }

    const sg_buffer _get_index_buffer() const {
        return m_index_buffer;
    }

    const sg_bindings get_bindings() const {
        sg_bindings bindings = {};
        bindings.vertex_buffers[0] = m_vertex_buffer;
        bindings.index_buffer = m_index_buffer;
        return bindings;
    }
};
} // namespace render

#endif
