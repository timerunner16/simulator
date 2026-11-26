/*
 *  core/type/monad.hxx
 *
 *  option and result types
 */

#ifndef simulator_core_type_monad_hxx
#define simulator_core_type_monad_hxx

#include "container.hxx"
#include <expected>
#include <functional>
#include <optional>
#include <print>
#include <system_error>

namespace core {
template <class T> class Option : public std::optional<T> {
  public:
    using std::optional<T>::optional;

    T &&expect(const String &prepend_msg) noexcept {
        if (!this->has_value()) {
            std::println("{}", prepend_msg);
            std::println("Aborting process.");
            std::abort();
        }
        return std::move(**this);
    }

    T &&unwrap() noexcept {
        if (!this->has_value()) {
            std::println("Unwrap called on an Option.");
            std::println("Aborting process.");
            std::abort();
        }
        return std::move(**this);
    }
};

template <class T> class Option<T &> : public std::optional<std::reference_wrapper<T>> {
  public:
    using std::optional<std::reference_wrapper<T>>::optional;
};

using ErrCode = std::error_code;
using ErrCategory = std::error_category;

template <class T> class Result : public std::expected<T, ErrCode> {
  public:
    using std::expected<T, ErrCode>::expected;

    auto expect(const String &prepend_msg) noexcept -> decltype(auto) {
        if (!this->has_value()) {
            std::println("{}: {}", prepend_msg, this->error().message());
            std::println("Aborting process.");
            std::abort();
        }
        using res_type = std::remove_cvref_t<decltype(*this)>;
        if constexpr (!std::is_void_v<typename res_type::value_type>) {
            return std::move(**this);
        }
    }

    auto unwrap() noexcept -> decltype(auto) {
        if (!this->has_value()) {
            std::println("Unwrap called on an error: {}", this->error().message());
            std::println("Aborting process.");
            std::abort();
        }
        using res_type = std::remove_cvref_t<decltype(*this)>;
        if constexpr (!std::is_void_v<typename res_type::value_type>) {
            return std::move(**this);
        }
    }
};

using Err = std::unexpected<ErrCode>;

#define ERRCODEMAKER_EXITNS_0
#define ERRCODEMAKER_EXITNS_1 }
#define ERRCODEMAKER_EXITNS_2                                                                      \
    }                                                                                              \
    ERRCODEMAKER_EXITNS_1
#define ERRCODEMAKER_EXITNS_3                                                                      \
    }                                                                                              \
    ERRCODEMAKER_EXITNS_2
#define ERRCODEMAKER_EXITNS_4                                                                      \
    }                                                                                              \
    ERRCODEMAKER_EXITNS_3
#define ERRCODEMAKER_EXITNS_5                                                                      \
    }                                                                                              \
    ERRCODEMAKER_EXITNS_4
#define ERRCODEMAKER_EXITNS_6                                                                      \
    }                                                                                              \
    ERRCODEMAKER_EXITNS_5

#define ERRCODEMAKER_OPENNS_0()
#define ERRCODEMAKER_OPENNS_1(ns) namespace ns {
#define ERRCODEMAKER_OPENNS_2(ns, ...)                                                             \
    namespace ns {                                                                                 \
    ERRCODEMAKER_OPENNS_1(__VA_ARGS__)
#define ERRCODEMAKER_OPENNS_3(ns, ...)                                                             \
    namespace ns {                                                                                 \
    ERRCODEMAKER_OPENNS_2(__VA_ARGS__)
#define ERRCODEMAKER_OPENNS_4(ns, ...)                                                             \
    namespace ns {                                                                                 \
    ERRCODEMAKER_OPENNS_3(__VA_ARGS__)
#define ERRCODEMAKER_OPENNS_5(ns, ...)                                                             \
    namespace ns {                                                                                 \
    ERRCODEMAKER_OPENNS_4(__VA_ARGS__)
#define ERRCODEMAKER_OPENNS_6(ns, ...)                                                             \
    namespace ns {                                                                                 \
    ERRCODEMAKER_OPENNS_5(__VA_ARGS__)

#define ERRCODEMAKER_EXITNS_N(n) ERRCODEMAKER_EXITNS_##n

#define ERRCODEMAKER_OPENNS_N(n, ...) ERRCODEMAKER_OPENNS_##n(__VA_ARGS__)

#define ERRCODEMAKER_MAKE_ENUM(name, id, msg) name = id,

#define ERRCODEMAKER_MAKE_MSG(name, id, msg)                                                       \
    case name :                                                                                    \
        return msg;

#define ERRCODEMAKER_JOINNS_0()
#define ERRCODEMAKER_JOINNS_1(ns) ::ns
#define ERRCODEMAKER_JOINNS_2(ns, ...) ::ns ERRCODEMAKER_JOINNS_1(__VA_ARGS__)
#define ERRCODEMAKER_JOINNS_3(ns, ...) ::ns ERRCODEMAKER_JOINNS_2(__VA_ARGS__)
#define ERRCODEMAKER_JOINNS_4(ns, ...) ::ns ERRCODEMAKER_JOINNS_3(__VA_ARGS__)
#define ERRCODEMAKER_JOINNS_5(ns, ...) ::ns ERRCODEMAKER_JOINNS_4(__VA_ARGS__)
#define ERRCODEMAKER_JOINNS_6(ns, ...) ::ns ERRCODEMAKER_JOINNS_5(__VA_ARGS__)

#define ERRCODEMAKER_JOINNS_N(n, ...) ERRCODEMAKER_JOINNS_##n(__VA_ARGS__)

#define ERRCODEMAKER_STRNS_0()
#define ERRCODEMAKER_STRNS_1(ns) "::" #ns
#define ERRCODEMAKER_STRNS_2(ns, ...) "::" #ns ERRCODEMAKER_STRNS_1(__VA_ARGS__)
#define ERRCODEMAKER_STRNS_3(ns, ...) "::" #ns ERRCODEMAKER_STRNS_2(__VA_ARGS__)
#define ERRCODEMAKER_STRNS_4(ns, ...) "::" #ns ERRCODEMAKER_STRNS_3(__VA_ARGS__)
#define ERRCODEMAKER_STRNS_5(ns, ...) "::" #ns ERRCODEMAKER_STRNS_4(__VA_ARGS__)
#define ERRCODEMAKER_STRNS_6(ns, ...) "::" #ns ERRCODEMAKER_STRNS_5(__VA_ARGS__)

#define ERRCODEMAKER_STRNS_N(n, ...) ERRCODEMAKER_STRNS_##n(__VA_ARGS__)

#define DEFINE_ERRCODE(enum_name, defs, depth, ...)                                                \
    enum class enum_name { defs(ERRCODEMAKER_MAKE_ENUM) };                                         \
                                                                                                   \
    namespace detail {                                                                             \
    class enum_name##_category : public ErrCategory {                                              \
      public:                                                                                      \
        virtual const char *name() const noexcept override final {                                 \
            return ERRCODEMAKER_STRNS_N(depth, __VA_ARGS__) "::" #enum_name;                       \
        }                                                                                          \
                                                                                                   \
        virtual String message(int c) const override final {                                       \
            using enum enum_name;                                                                  \
            switch (static_cast<enum_name>(c)) {                                                   \
                defs(ERRCODEMAKER_MAKE_MSG) default : return "unknown";                            \
            }                                                                                      \
        }                                                                                          \
    };                                                                                             \
    }                                                                                              \
                                                                                                   \
    extern inline const detail::enum_name##_category &enum_name##_category() {                     \
        static detail::enum_name##_category c;                                                     \
        return c;                                                                                  \
    }                                                                                              \
                                                                                                   \
    inline ::core::ErrCode make_error_code(enum_name e) {                                          \
        return {static_cast<int>(e), enum_name##_category()};                                      \
    }                                                                                              \
                                                                                                   \
    ERRCODEMAKER_EXITNS_N(depth)                                                                   \
    namespace std {                                                                                \
    template <>                                                                                    \
    struct is_error_code_enum<ERRCODEMAKER_JOINNS_N(depth, __VA_ARGS__)::enum_name> : true_type {  \
    };                                                                                             \
    }                                                                                              \
    ERRCODEMAKER_OPENNS_N(depth, __VA_ARGS__)

#define TRY(failable)                                                                              \
    ({                                                                                             \
        auto &&result = (failable);                                                                \
        if (!result.has_value())                                                                   \
            return Err(result.error());                                                            \
        [](auto &&res) -> decltype(auto) {                                                         \
            using res_type = std::remove_cvref_t<decltype(res)>;                                   \
            if constexpr (!std::is_void_v<typename res_type::value_type>) {                        \
                return std::move(*res);                                                            \
            }                                                                                      \
        }(result);                                                                                 \
    })
} // namespace core

#endif
