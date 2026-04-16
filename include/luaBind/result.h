#pragma once

#include <core/expected.h>

namespace Typhoon::LuaBind {

using Result = Expected<void, const char*>;

template <typename T>
using ResultT = Expected<T, const char*>;

} // namespace Typhoon::LuaBind
