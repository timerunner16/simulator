/*
 *  render/sampler.hxx
 *
 *  c++ wrapper over sokol samplers
 */

#ifndef simulator_render_sampler_hxx
#define simulator_render_sampler_hxx

#include <core/core.hxx>

#include <sokol_gfx.h>

namespace render {
using namespace core;

class Sampler {
  private:
    sg_sampler m_sampler;

    explicit Sampler(sg_sampler sampler) : m_sampler(sampler) {}

  public:
    ~Sampler() {
        sg_destroy_sampler(m_sampler);
    }

    Sampler(const Sampler &other) = delete;

    Sampler &operator=(const Sampler &other) = delete;

    Sampler(Sampler &&other) : m_sampler(other.m_sampler) {
        other.m_sampler = {};
    }

    Sampler &operator=(Sampler &&other) {
        m_sampler = other.m_sampler;

        other.m_sampler = {};

        return *this;
    }

    const sg_sampler &_get_sampler() const {
        return m_sampler;
    }

    enum class Filter {
        LINEAR,
        NEAREST,
    };

    enum class Border {
        BLACK,
        WHITE,
        TRANSPARENT,
    };

    enum class Wrap {
        CLAMP,
        REPEAT,
    };

    static Option<Sampler> create(Filter filter, Border border, Wrap wrap) {
        sg_sampler_desc sampler_desc = {};
        sampler_desc.mag_filter = filter == Filter::LINEAR ? SG_FILTER_LINEAR : SG_FILTER_NEAREST;
        sampler_desc.min_filter = sampler_desc.mag_filter;
        sampler_desc.mipmap_filter = sampler_desc.mag_filter;
        if (border == Border::BLACK)
            sampler_desc.border_color = SG_BORDERCOLOR_OPAQUE_BLACK;
        else if (border == Border::WHITE)
            sampler_desc.border_color = SG_BORDERCOLOR_OPAQUE_WHITE;
        else
            sampler_desc.border_color = SG_BORDERCOLOR_TRANSPARENT_BLACK;
        sampler_desc.wrap_u = wrap == Wrap::CLAMP ? SG_WRAP_CLAMP_TO_BORDER : SG_WRAP_REPEAT;
        sampler_desc.wrap_v = wrap == Wrap::CLAMP ? SG_WRAP_CLAMP_TO_BORDER : SG_WRAP_REPEAT;
        const sg_sampler sampler = sg_make_sampler(sampler_desc);
        if (sg_query_sampler_state(sampler) != SG_RESOURCESTATE_VALID)
            return {};

        return Sampler(sampler);
    }
};
}; // namespace render

#endif
