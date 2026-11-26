/*
 *  render/mesh.hxx
 *
 *  mesh storage, loading, management
 */

#ifndef simulator_render_mesh_hxx
#define simulator_render_mesh_hxx

#include <sokol_gfx.h>

#include <core/core.hxx>

template <> struct std::default_delete<const sg_buffer> {
    constexpr default_delete() = default;

    void operator()(const sg_buffer *buffer) const noexcept {
        if (buffer) {
            sg_destroy_buffer(*buffer);
        }
    }
};

namespace render {
using namespace core;

#define MESH_ERROR(X)                                                                              \
    X(vertex_buffer_failed, 1, "vertex buffer failed to construct")                                \
    X(index_buffer_failed, 2, "index buffer failed to construct")

DEFINE_ERRCODE(MeshError, MESH_ERROR, 1, render)
#undef MESH_ERROR

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
    Unique<const sg_buffer> m_vertex_buffer;
    Unique<const sg_buffer> m_index_buffer;

    const usize m_index_count;

    explicit Mesh(Unique<const sg_buffer> &&vertex_buffer, Unique<const sg_buffer> &&index_buffer,
                  const usize index_count)
        : m_vertex_buffer(std::move(vertex_buffer)), m_index_buffer(std::move(index_buffer)),
          m_index_count(index_count) {};

  public:
    static Result<Mesh> create_quad(Vec3 p, Vec2 s, bool colored);

    const sg_buffer _get_vertex_buffer() const {
        return *m_vertex_buffer;
    }

    const sg_buffer _get_index_buffer() const {
        return *m_index_buffer;
    }

    const usize _get_index_count() const {
        return m_index_count;
    }
};
} // namespace render

#endif
