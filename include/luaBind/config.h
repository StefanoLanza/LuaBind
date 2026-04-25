#pragma once

#if __cplusplus < 201703L
#error "LuaBind requires C++ 17 or above"
#endif

// Set to 1 to check pointer types at runtime
#ifndef TY_LUABIND_TYPE_SAFE
#define TY_LUABIND_TYPE_SAFE 0
#endif

// Set to 1 to create the LuaBind alias for the namespace Typhoon::LuaBind
#ifndef TY_LUABIND_NAMESPACE_ALIAS
#define TY_LUABIND_NAMESPACE_ALIAS 1
#endif
