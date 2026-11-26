/*
 *  core/type/function.hxx
 *
 *  functional types
 */

#ifndef simulator_core_type_function_hxx
#define simulator_core_type_function_hxx

#include <functional>

namespace core {
template <typename Return, typename... Args> using Fn = std::function<Return(Args...)>;
}

#endif
