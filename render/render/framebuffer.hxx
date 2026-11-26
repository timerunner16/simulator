/*
 *  render/framebuffer.hxx
 *
 *  color/depth framebuffers
 */

#ifndef simulator_render_framebuffer_hxx
#define simulator_render_framebuffer_hxx

#include <core/core.hxx>

#include "image.hxx"

namespace render {
using namespace core;

#define FRAMEBUFFER_ERROR(X) X(invalid_dimensions, 1, "width and height must be greater than 0")

DEFINE_ERRCODE(FramebufferError, FRAMEBUFFER_ERROR, 1, render)

#undef FRAMEBUFFER_ERROR

class Framebuffer {
  private:
    Shared<Image> m_color;
    Shared<Image> m_depth;

    Vec2 m_dims;

    explicit Framebuffer(Shared<Image> color, Shared<Image> depth, const Vec2 &dims) noexcept
        : m_color(color), m_depth(depth), m_dims(dims) {};

  public:
    static Result<Framebuffer> create(const Vec2 &dims);

    Vec2 get_size() {
        return m_dims;
    }

    const Shared<Image> _get_color() {
        return m_color;
    }

    const Shared<Image> _get_depth() {
        return m_depth;
    }
};
} // namespace render

#endif
