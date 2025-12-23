#pragma once

#if __has_include(<expected>) && _HAS_CXX23
#include <expected>

template <typename T, typename E>
using expected_t = std::expected<T, E>;

#define UNEXPECTED std::unexpected

using Result = std::expected<void, const char*>;

template <typename T>
using ResultT = std::expected<T, const char*>;

#else

#include <expected/include/tl/expected.hpp>

template <typename T, typename E>
using expected_t = tl::expected<T, E>;
#define UNEXPECTED tl::make_unexpected
using Result = tl::expected<void, const char*>;

template <typename T>
using ResultT = tl::expected<T, const char*>;

#endif
