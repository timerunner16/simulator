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

class Framebuffer {
  private:
    Image m_color;
    Image m_depth;

    Vec2 m_dims;

    explicit Framebuffer(Image color, Image depth, const Vec2 &dims)
        : m_color(std::move(color)), m_depth(std::move(depth)), m_dims(dims) {};

  public:
    static Option<Framebuffer> create(const Vec2 &dims) {
        if (dims.x < 0 || dims.y < 0)
            return {};

        auto color_opt = Image::create_empty_color(dims);
        if (!color_opt.has_value())
            return {};
        Image color = std::move(*color_opt);

        auto depth_opt = Image::create_empty_depth(dims);
        if (!depth_opt.has_value())
            return {};
        Image depth = std::move(*depth_opt);

        return Framebuffer(std::move(color), std::move(depth), dims);
    }

    Vec2 get_size() {
        return m_dims;
    }

    const Image &_get_color() {
        return m_color;
    }

    const Image &_get_depth() {
        return m_depth;
    }
};
} // namespace render

#endif
