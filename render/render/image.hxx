/*
 *  render/image.hxx
 *
 *  c++ wrapper over sokol images/views
 */

#ifndef simulator_render_image_hxx
#define simulator_render_image_hxx

#include <core/core.hxx>

#include <sokol_gfx.h>

template <> struct std::default_delete<const sg_image> {
    constexpr default_delete() = default;

    void operator()(const sg_image *image) const noexcept {
        if (image) {
            sg_destroy_image(*image);
        }
    }
};

template <> struct std::default_delete<const sg_view> {
    constexpr default_delete() = default;

    void operator()(const sg_view *view) const noexcept {
        if (view) {
            sg_destroy_view(*view);
        }
    }
};

namespace render {
using namespace core;

#define IMAGE_ERROR(X)                                                                             \
    X(invalid_dimensions, 1, "width and height must be greater than 0")                            \
    X(image_failed, 2, "image failed to construct")                                                \
    X(view_failed, 3, "view failed to construct")

DEFINE_ERRCODE(ImageError, IMAGE_ERROR, 1, render)
#undef IMAGE_ERROR

class Image {
  private:
    Unique<const sg_image> m_image;
    Unique<const sg_view> m_att_view;
    Unique<const sg_view> m_tex_view;

    Vec2 m_dims;

    explicit Image(Unique<const sg_image> &&image, Unique<const sg_view> &&att_view,
                   Unique<const sg_view> &&tex_view, Vec2 dims) noexcept
        : m_image(std::move(image)), m_att_view(std::move(att_view)),
          m_tex_view(std::move(tex_view)), m_dims(dims) {}

  public:
    const sg_view &_get_att_view() const {
        return *m_att_view.get();
    }

    const sg_view &_get_tex_view() const {
        return *m_tex_view.get();
    }

    static Result<Image> create_fill(const Vec2 &dims, u8 r, u8 g, u8 b, u8 a);

    static Result<Image> create_xor();

    static Result<Image> create_empty_color(const Vec2 &dims);
    static Result<Image> create_empty_depth(const Vec2 &dims);
};
} // namespace render

#endif
