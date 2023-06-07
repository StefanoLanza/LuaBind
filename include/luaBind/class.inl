#pragma once

#include "config.h"

#include "boxing.h"
#include "detail.h"
#include "reference.h"
#include <core/typeId.h>
#include <type_traits>

namespace Typhoon::LuaBind::detail {

constexpr int kLuaAllocated = 0;
constexpr int kCppAllocated = 1;

template <typename T, typename... argType>
T* defaultNew(argType... args) {
	return new T { args... };
}

// Create a new object and return it to Lua as a full user data
template <typename retType, typename... argType, std::size_t... argIndices>
int wrapNewImpl(lua_State* ls, std::integer_sequence<std::size_t, argIndices...> indx) {
	// Extract function pointer from upvalue
	using func_ptr = retType (*)(argType...);
	const void* const func_ud = lua_touserdata(ls, lua_upvalueindex(1));
	const func_ptr    func = serializePOD<func_ptr>(func_ud);

	// Get stack size of all arguments
	// Because of C++ rules, by creating an array GetStackSize is called in the correct order for each argument
	const int argStackSize[] = { getStackSize<argType>()..., 0 };

	// Compute stack indices
	int argStackIndex[sizeof...(argType) + 1] = {};
	argStackIndex[0] = 1;
	for (size_t i = 1; i < sizeof...(argType); ++i) {
		argStackIndex[i] = argStackIndex[i - 1] + argStackSize[i - 1];
	}

	// Check arguments
	checkArgs<argType...>(ls, argStackIndex, indx);

	// Pop and pass args
	const auto obj = func(pop<argType>(ls, argStackIndex[argIndices])...);

	if constexpr (isLightweight<retType>) {
		// Push lightweight object as light user data
		Lightweight<retType>::push(ls, obj);
	}
	else {
		// Allocate full user data and store the object pointer in it
		void* const ud = lua_newuserdatauv(ls, sizeof obj, 1);
		std::memcpy(ud, &obj, sizeof obj);

		// Associate metatable
		const auto className = typeName<std::remove_pointer_t<retType>>();
		assert(className);
		luaL_getmetatable(ls, className);
		lua_setmetatable(ls, -2);

		// Mark as heap allocated by Lua. This user value is queried in wrapDefaultDelete<T>
		lua_pushinteger(ls, kLuaAllocated);
		lua_setiuservalue(ls, -2, 1); // ud.userValue[1] = kLuaAllocated

#if TY_LUABIND_TYPE_SAFE
		registerPointer(ls, obj);
#endif
	}
	return 1;
}

template <typename retType, typename... argType>
int wrapNew(lua_State* ls) {
	return wrapNewImpl<retType, argType...>(ls, std::index_sequence_for<argType...> {});
}

template <class T>
int wrapDefaultDelete(lua_State* ls) {
	if (isAllocatedByLua(ls, 1)) {
		// Extract pointer from user data
		T* obj = nullptr;
		std::memcpy(&obj, lua_touserdata(ls, 1), sizeof obj);
		assert(obj);
		delete obj; // object allocated by Lua
	}
	// else object was allocated by C++
	return 0;
}

template <class T>
int wrapDeleter(lua_State* ls) {
	if (isAllocatedByLua(ls, 1)) {
		// Extract pointer from user data
		T* obj = nullptr;
		std::memcpy(&obj, lua_touserdata(ls, 1), sizeof obj);
		assert(obj);

		// Extract function pointer
		const void* const ud = lua_touserdata(ls, lua_upvalueindex(1));
		using Deleter = void (*)(T*);
		const auto deleter = serializePOD<Deleter>(ud);

		deleter(obj); // object allocated by Lua
	}
	// else object was allocated by C++
	return 0;
}

template <class T>
Reference registerCppClass(lua_State* ls, const char* className, TypeId baseClassId) {
	const Reference ref = registerCppClass(ls, className, Typhoon::getTypeId<T>(), baseClassId);
	if constexpr (isLightweight<T>) {
		// Lightweight types get boxing functions
		lua_rawgeti(ls, LUA_REGISTRYINDEX, ref.getValue()); // table
		const int tableStackIndex = lua_gettop(ls);
		pushBoxingFunctions<T>(ls, tableStackIndex);
		lua_pop(ls, 1);
	}
	return ref;
}

template <typename classType, typename retType, typename... argType>
inline void registerNewOperator(lua_State* ls, int classTableStackIndex, retType (*functionPtr)(argType...)) {
	static_assert(std::is_same_v<classType, std::remove_pointer_t<retType>>, "Invalid return type for new operator");
	void* buffer = lua_newuserdatauv(ls, sizeof functionPtr, 0);
	std::memcpy(buffer, &functionPtr, sizeof functionPtr);
	lua_pushcclosure(ls, wrapNew<retType, argType...>, 1);
	lua_setfield(ls, classTableStackIndex, "new"); // table.new = wrapNew
}

template <typename classType, typename argType>
inline void registerDeleteOperator(lua_State* ls, int classTableStackIndex, void (*functionPtr)(argType*)) {
	static_assert(std::is_same_v<classType, argType>, "Invalid argument type for delete operator");
	// Store pointer to delete function as upvalue of wrapDeleter<argType>
	void* buffer = lua_newuserdatauv(ls, sizeof functionPtr, 0);
	std::memcpy(buffer, &functionPtr, sizeof functionPtr);
	lua_pushcclosure(ls, wrapDeleter<argType>, 1);
	lua_setfield(ls, classTableStackIndex, "__gc"); // mt.__gc = wrapDeleter
}

template <typename T, typename... argType>
void registerDefaultNewOperator(lua_State* ls, int classTableStackIndex) {
	auto actualNew = defaultNew<T, argType...>;
	registerNewAndDeleteOperators(ls, classTableStackIndex, wrapNew<T*, argType...>, wrapDefaultDelete<T>, &actualNew, sizeof actualNew);
}

} // namespace Typhoon::LuaBind::detail
