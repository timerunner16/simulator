/*
 *  render/image.hxx
 *
 *  c++ wrapper over sokol images/views
 */

#ifndef simulator_render_image_hxx
#define simulator_render_image_hxx

#include <core/core.hxx>

#include <sokol_gfx.h>

#include <array>
#include <bit>

namespace render {
using namespace core;

class Image {
  private:
    sg_image m_image;
    sg_view m_att_view;
    sg_view m_tex_view;

    Vec2 m_dims;

    explicit Image(sg_image image, sg_view att_view, sg_view tex_view, Vec2 dims)
        : m_image(image), m_att_view(att_view), m_tex_view(tex_view), m_dims(dims) {}

  public:
    ~Image() {
        sg_destroy_image(m_image);
        sg_destroy_view(m_att_view);
        sg_destroy_view(m_tex_view);
    }

    Image(const Image &other) = delete;
    Image &operator=(const Image &other) = delete;

    Image(Image &&other)
        : m_image(other.m_image), m_att_view(other.m_att_view), m_tex_view(other.m_tex_view),
          m_dims(other.m_dims) {
        other.m_image = {};
        other.m_att_view = {};
        other.m_tex_view = {};
        other.m_dims = {0, 0};
    }

    Image &operator=(Image &&other) {
        m_image = other.m_image;
        m_att_view = other.m_att_view;
        m_tex_view = other.m_tex_view;
        m_dims = other.m_dims;

        other.m_image = {};
        other.m_att_view = {};
        other.m_tex_view = {};
        other.m_dims = {0, 0};

        return *this;
    }

    const sg_view &_get_att_view() const {
        return m_att_view;
    }

    const sg_view &_get_tex_view() const {
        return m_tex_view;
    }

    static Option<Image> create_fill(const Vec2 &dims, u8 r, u8 g, u8 b, u8 a) {
        if (dims.x <= 0 || dims.y <= 0)
            return {};

        std::vector<u32> data(dims.x * dims.y, std::bit_cast<u32, u8[4]>({r, g, b, a}));

        sg_image_desc image_desc = {};
        image_desc.width = dims.x;
        image_desc.height = dims.y;
        image_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
        image_desc.sample_count = 1;
        image_desc.data = {
            sg_range{.ptr = data.data(), .size = 4 * data.size()}
        };
        const sg_image image = sg_make_image(image_desc);
        if (sg_query_image_state(image) != SG_RESOURCESTATE_VALID)
            return {};

        sg_view_desc tex_view_desc = {};
        tex_view_desc.texture.image = image;
        const sg_view tex_view = sg_make_view(tex_view_desc);
        if (sg_query_view_state(tex_view) != SG_RESOURCESTATE_VALID)
            return {};

        return Image(image, {}, tex_view, dims);
    }

    static Option<Image> create_xor() {
        std::array<u8, 4 * 256 * 256> data;
        for (i32 y = 0; y < 256; y++) {
            for (i32 x = 0; x < 256; x++) {
                usize id = (y * 256 + x) * 4;
                u8 c = x ^ y;

                data[id] = c;
                data[id + 1] = c;
                data[id + 2] = c;
                data[id + 3] = 255;
            }
        }

        sg_image_desc image_desc = {};
        image_desc.width = 256;
        image_desc.height = 256;
        image_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
        image_desc.data = {
            sg_range{.ptr = data.data(), .size = data.size()}
        };
        const sg_image image = sg_make_image(image_desc);
        if (sg_query_image_state(image) != SG_RESOURCESTATE_VALID)
            return {};

        sg_view_desc tex_view_desc = {};
        tex_view_desc.texture.image = image;
        const sg_view tex_view = sg_make_view(tex_view_desc);
        if (sg_query_view_state(tex_view) != SG_RESOURCESTATE_VALID)
            return {};

        return Image(image, {}, tex_view, Vec2(256, 256));
    }

    static Option<Image> create_empty_color(const Vec2 &dims) {
        if (dims.x <= 0 || dims.y <= 0)
            return {};

        sg_image_desc image_desc = {};
        image_desc.usage.color_attachment = true;
        image_desc.width = dims.x;
        image_desc.height = dims.y;
        image_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
        image_desc.sample_count = 1;
        image_desc.label = 0;
        const sg_image image = sg_make_image(image_desc);
        if (sg_query_image_state(image) != SG_RESOURCESTATE_VALID)
            return {};

        sg_view_desc att_view_desc = {};
        att_view_desc.color_attachment.image = image;
        const sg_view att_view = sg_make_view(att_view_desc);
        if (sg_query_view_state(att_view) != SG_RESOURCESTATE_VALID)
            return {};

        sg_view_desc tex_view_desc = {};
        tex_view_desc.texture.image = image;
        const sg_view tex_view = sg_make_view(tex_view_desc);
        if (sg_query_view_state(tex_view) != SG_RESOURCESTATE_VALID)
            return {};

        return Image(image, att_view, tex_view, dims);
    }

    static Option<Image> create_empty_depth(const Vec2 &dims) {
        if (dims.x <= 0 || dims.y <= 0)
            return {};

        sg_image_desc image_desc = {};
        image_desc.usage.depth_stencil_attachment = true;
        image_desc.width = dims.x;
        image_desc.height = dims.y;
        image_desc.pixel_format = SG_PIXELFORMAT_DEPTH;
        image_desc.sample_count = 1;
        image_desc.label = 0;
        const sg_image image = sg_make_image(image_desc);
        if (sg_query_image_state(image) != SG_RESOURCESTATE_VALID)
            return {};

        sg_view_desc att_view_desc = {};
        att_view_desc.depth_stencil_attachment.image = image;
        const sg_view att_view = sg_make_view(att_view_desc);
        if (sg_query_view_state(att_view) != SG_RESOURCESTATE_VALID)
            return {};

        sg_view_desc tex_view_desc = {};
        tex_view_desc.texture.image = image;
        const sg_view tex_view = sg_make_view(tex_view_desc);
        if (sg_query_view_state(tex_view) != SG_RESOURCESTATE_VALID)
            return {};

        return Image(image, att_view, tex_view, dims);
    }
};
} // namespace render

#endif
