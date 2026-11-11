/*
 *  render/image.hxx
 *
 *  c++ wrapper over sokol images/views
 */

#ifndef simulator_render_image_hxx
#define simulator_render_image_hxx

#include <core/core.hxx>

#include <sokol_gfx.h>

namespace render {
using namespace core;

class Image {
  private:
    const sg_image m_image;
    const sg_view m_att_view;
    const sg_view m_tex_view;

    const Vec2 m_dims;

    explicit Image(sg_image image, sg_view att_view, sg_view tex_view)
        : m_image(image), m_att_view(att_view), m_tex_view(tex_view) {}

  public:
    ~Image() {
        sg_destroy_image(m_image);
        sg_destroy_view(m_att_view);
        sg_destroy_view(m_tex_view);
    }

    const sg_view &_get_att_view() const {
        return m_att_view;
    }

    const sg_view &_get_tex_view() const {
        return m_tex_view;
    }

    static Option<Image> create_empty_color(const Vec2 &dims) {
        if (dims.x < 0 || dims.y < 0)
            return {};

        sg_image_desc image_desc = {};
        image_desc.usage.color_attachment = true;
        image_desc.width = dims.x;
        image_desc.height = dims.y;
        image_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
        image_desc.sample_count = 1;
        image_desc.label = 0;
        const sg_image image = sg_make_image(image_desc);

        sg_view_desc att_view_desc = {};
        att_view_desc.color_attachment.image = image;
        const sg_view att_view = sg_make_view(att_view_desc);

        sg_view_desc tex_view_desc = {};
        tex_view_desc.texture.image = image;
        const sg_view tex_view = sg_make_view(tex_view_desc);

        return Image(image, att_view, tex_view);
    }

    static Option<Image> create_empty_depth(const Vec2 &dims) {
        if (dims.x < 0 || dims.y < 0)
            return {};

        sg_image_desc image_desc = {};
        image_desc.usage.depth_stencil_attachment = true;
        image_desc.width = dims.x;
        image_desc.height = dims.y;
        image_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
        image_desc.sample_count = 1;
        image_desc.label = 0;
        const sg_image image = sg_make_image(image_desc);

        sg_view_desc att_view_desc = {};
        att_view_desc.depth_stencil_attachment.image = image;
        const sg_view att_view = sg_make_view(att_view_desc);

        sg_view_desc tex_view_desc = {};
        tex_view_desc.texture.image = image;
        const sg_view tex_view = sg_make_view(tex_view_desc);

        return Image(image, att_view, tex_view);
    }
};
} // namespace render

#endif
