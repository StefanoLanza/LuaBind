#pragma once

#include "reference.h"
#include <cassert>
#include <core/typeId.h>

namespace Typhoon::LuaBind {

Reference registerObjectAsTable(lua_State* ls, void* objectPtr, TypeId typeId);

Reference registerObjectAsUserData(lua_State* ls, void* objectPtr, TypeId typeId);

Reference registerObjectAsLightUserData(lua_State* ls, void* objectPtr, TypeId typeId);

Reference registerTable(lua_State* ls, const char* className);

void unregisterObject(lua_State* ls, Reference ref);

void unregisterObject(lua_State* ls, void* objectPtr);

template <class T>
Reference registerObjectAsUserData(lua_State* ls, T* objectPtr) {
	assert(objectPtr);
	const auto typeId = getTypeId(objectPtr);
	return registerObjectAsUserData(ls, static_cast<void*>(objectPtr), typeId);
}

template <class T>
Reference registerObjectAsLightUserData(lua_State* ls, T* objectPtr) {
	assert(objectPtr);
	const auto typeId = getTypeId(objectPtr);
	return registerObjectAsLightUserData(ls, static_cast<void*>(objectPtr), typeId);
}

template <class T>
Reference registerObjectAsTable(lua_State* ls, T* objectPtr) {
	assert(objectPtr);
	const auto typeId = getTypeId(objectPtr);
	return registerObjectAsTable(ls, static_cast<void*>(objectPtr), typeId);
}

} // namespace Typhoon::LuaBind