/*
 *  render/sampler.hxx
 *
 *  c++ wrapper over sokol samplers
 */

#ifndef simulator_render_sampler_hxx
#define simulator_render_sampler_hxx

#include <core/core.hxx>

#include <sokol_gfx.h>

template <> struct std::default_delete<const sg_sampler> {
    constexpr default_delete() = default;

    void operator()(const sg_sampler *sampler) const noexcept {
        if (sampler) {
            sg_destroy_sampler(*sampler);
        }
    }
};

namespace render {
using namespace core;

#define SAMPLER_ERROR(X) X(sampler_failed, 1, "sampler failed to construct")

DEFINE_ERRCODE(SamplerError, SAMPLER_ERROR, 1, render)
#undef SAMPLER_ERROR

class Sampler {
  private:
    Unique<const sg_sampler> m_sampler;

    explicit Sampler(Unique<const sg_sampler> &&sampler) : m_sampler(std::move(sampler)) {}

  public:
    const sg_sampler &_get_sampler() const {
        return *m_sampler;
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

    static Result<Sampler> create(Filter filter, Border border, Wrap wrap);
};
}; // namespace render

#endif
