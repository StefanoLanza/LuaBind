#pragma once

#include <version>
#ifdef __cpp_lib_expected
#include <expected>

namespace Typhoon::LuaBind {

template <typename T, typename E>
using Expected = std::expected<T, E>;

#define UNEXPECTED std::unexpected

using Result = std::expected<void, const char*>;

template <typename T>
using ResultT = std::expected<T, const char*>;

}

#else

#include <core/expected.hpp>

namespace Typhoon::LuaBind {

template <typename T, typename E>
using Expected = tl::expected<T, E>;

#define UNEXPECTED tl::make_unexpected
using Result = tl::expected<void, const char*>;

template <typename T>
using ResultT = tl::expected<T, const char*>;

}

#endif
