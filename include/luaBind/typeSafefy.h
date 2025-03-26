#pragma once

#include "config.h"

#if TY_LUABIND_TYPE_SAFE

struct lua_State;

#include <cassert>
#include <core/typeId.h>

namespace Typhoon::LuaBind::detail {

void  registerBaseClass(lua_State* ls, TypeId super, TypeId base);
bool  checkPointerType(lua_State* ls, const void* ptr, TypeId typeId);
void  registerTemporaryPointer(lua_State* ls, const void* ptr, TypeId typeId);
void* makePointerKey(const void* ptr, TypeId typeId);
bool compatibleTypes(lua_State* ls, TypeId first, TypeId second);

} // namespace Typhoon::LuaBind::detail

#endif
