#pragma once

#include "nil.h"
#include "reference.h"
#include "stackIndex.h"
#include "typeSafefy.h"

#include <cassert>
#include <cstring>
#include <limits>
#include <lua/src/lua.hpp>
#include <string>

#if __cplusplus >= 202002L
#include <concepts>
#endif

namespace Typhoon::LuaBind {

namespace detail {

constexpr int kLuaAllocated = 0;
constexpr int kCppAllocated = 1;

lua_Integer makePointerKey(const void* ptr, TypeId typeId);

} // namespace detail

// typename = void is used for specializations based on std::enable_if. See the enum specialization
// Generic wrapper
template <class T, typename = void>
class Wrapper {
public:
	static constexpr int getStackSize() {
		return 1;
	}

	static int match(lua_State* ls, int idx) {
		return lua_type(ls, idx) == LUA_TUSERDATA;
	}

	static void push(lua_State* ls, const T& value) {
		// Alloc and construct a copy of value
		T* const ptr = allocTemporary<T>(ls, value);
		Wrapper<T*>::push(ls, ptr);
	}

	static const T& pop(lua_State* ls, int idx) {
		void* userData = lua_touserdata(ls, idx);
		assert(userData);
		T* ptr = nullptr;
		std::memcpy(&ptr, userData, sizeof ptr);

#if TY_LUABIND_TYPE_SAFE
		// Check type
		lua_getiuservalue(ls, idx, 1);
		assert(! lua_isnil(ls, -1));
		TypeId ptrTypeId;
		ptrTypeId.impl = reinterpret_cast<const void*>(lua_tointeger(ls, -1));
		if (! detail::compatibleTypes(ls, ptrTypeId, getTypeId<T>())) {
			luaL_argerror(ls, idx, "Invalid pointer type");
			ptr = nullptr;
		}
#endif

		return *ptr;
	}
};

// Helper to push and pop temporary objects as light user data
template <class T>
struct Lightweight {
	static_assert(std::is_trivially_destructible_v<T>, "Lightweight trait requires a trivially destructible class");

	static constexpr int getStackSize() {
		return 1;
	}
	static int match(lua_State* ls, int idx) {
		return lua_isuserdata(ls, idx);
	}
	static void push(lua_State* ls, const T& value) {
		T* const ud = allocTemporary<T>(ls, value);
		if (ud) {
			lua_pushlightuserdata(ls, ud);
#if TY_LUABIND_TYPE_SAFE
			detail::registerTemporaryPointer(ls, ud, getTypeId<T>());
#endif
		}
		else {
			lua_pushnil(ls);
		}
	}

	static void pushAsKey(lua_State* ls, const T& value) {
		push(ls, value);
	}

	static const T& pop(lua_State* ls, int idx) {
		void* userData = lua_touserdata(ls, idx);
		assert(userData);
#if TY_LUABIND_TYPE_SAFE
		if (! detail::checkPointerType(ls, userData, getTypeId<T>())) {
			luaL_argerror(ls, idx, "Invalid pointer type");
		}
#endif
		return *static_cast<const T*>(userData);
	}
};

// Traits
template <class T>
inline constexpr bool isLightweight = std::is_base_of_v<Lightweight<T>, Wrapper<T>>;

template <>
class Wrapper<Nil> {
public:
	static constexpr int getStackSize() {
		return 1;
	}
	static int match(lua_State* ls, int idx) {
		return lua_isnil(ls, idx);
	}
	static void push(lua_State* ls, Nil /*nil*/) {
		lua_pushnil(ls);
	}
	static void pushAsKey(lua_State* ls, Nil /*nil*/) {
		lua_pushnil(ls);
	}
	static Nil pop([[maybe_unused]] lua_State* ls, [[maybe_unused]] int idx) {
		return Nil {};
	}
};

template <class I>
#if __cplusplus >= 202002L
requires std::integral<I>
#endif
    class IntegerWrapper {
public:
	static constexpr int getStackSize() {
		return 1;
	}
	static int match(lua_State* ls, int idx) {
		return lua_isinteger(ls, idx);
	}
	static void pushAsKey(lua_State* ls, I arg) {
		push(ls, arg);
	}
	static void push(lua_State* ls, I arg) {
		lua_pushinteger(ls, static_cast<lua_Integer>(arg));
	}
	static I pop(lua_State* ls, int idx) {
		lua_Integer i = lua_tointeger(ls, idx);
		if (i < static_cast<lua_Integer>(std::numeric_limits<I>::min()) && i > static_cast<lua_Integer>(std::numeric_limits<I>::max())) {
			luaL_error(ls, "Invalid cast from %d", i);
		}
		return static_cast<I>(i);
	}
};

template <class F>
#if __cplusplus >= 202002L
requires std::floating_point<F>
#endif
    class FloatWrapper {
public:
	static constexpr int getStackSize() {
		return 1;
	}
	static int match(lua_State* ls, int idx) {
		return lua_isnumber(ls, idx);
	}
	static void pushAsKey(lua_State* ls, F arg) {
		push(ls, arg);
	}
	static void push(lua_State* ls, F arg) {
		lua_pushnumber(ls, static_cast<lua_Number>(arg));
	}
	static F pop(lua_State* ls, int idx) {
		return static_cast<F>(lua_tonumber(ls, idx));
	}
};

template <>
class Wrapper<char> : public IntegerWrapper<char> {};

template <>
class Wrapper<unsigned char> : public IntegerWrapper<unsigned char> {};

template <>
class Wrapper<short> : public IntegerWrapper<short> {};

template <>
class Wrapper<unsigned short> : public IntegerWrapper<unsigned short> {};

template <>
class Wrapper<int> : public IntegerWrapper<int> {};

template <>
class Wrapper<unsigned int> : public IntegerWrapper<unsigned int> {};

template <>
class Wrapper<long> : public IntegerWrapper<long> {};

template <>
class Wrapper<unsigned long> : public IntegerWrapper<unsigned long> {};

template <>
class Wrapper<long long> : public IntegerWrapper<long long> {};

template <>
class Wrapper<unsigned long long> : public IntegerWrapper<unsigned long long> {};

template <>
class Wrapper<double> : public FloatWrapper<double> {};

template <>
class Wrapper<float> : public FloatWrapper<float> {};

template <>
class Wrapper<bool> {
public:
	static constexpr int getStackSize() {
		return 1;
	}
	static int match(lua_State* ls, int idx) {
		return lua_isboolean(ls, idx);
	}
	static void push(lua_State* ls, bool arg) {
		lua_pushboolean(ls, arg ? 1 : 0);
	}
	static bool pop(lua_State* ls, int idx) {
		return lua_toboolean(ls, idx) ? true : false;
	}
};

template <>
class Wrapper<const char*> {
public:
	static constexpr int getStackSize() {
		return 1;
	}
	static int match(lua_State* ls, int idx) {
		return lua_type(ls, idx) == LUA_TSTRING;
	}
	static void pushAsKey(lua_State* ls, const char* arg) {
		lua_pushstring(ls, arg);
	}
	static void push(lua_State* ls, const char* arg) {
		lua_pushstring(ls, arg);
	}
	static const char* pop(lua_State* ls, int idx) {
		return lua_tostring(ls, idx);
	}
};

// C literal string
template <size_t Size>
class Wrapper<const char (&)[Size]> : public Wrapper<const char*> {};

//! Raw pointer wrapper
template <class T>
class Wrapper<T*> {
public:
	static constexpr int getStackSize() {
		return 1;
	}
	static int match(lua_State* ls, int idx) {
		const int luaType = lua_type(ls, idx);
		return (luaType == LUA_TLIGHTUSERDATA || luaType == LUA_TUSERDATA || luaType == LUA_TTABLE || luaType == LUA_TNIL);
	}

	static void pushAsKey(lua_State* ls, T* ptr) {
		if (ptr) {
			lua_pushlightuserdata(ls, const_cast<non_const_ptr>(ptr));
		}
		else {
			lua_pushnil(ls);
		}
	}

	static void push(lua_State* ls, T* ptr) {
		if (! ptr) {
			lua_pushnil(ls);
			return;
		}

		if constexpr (std::is_void_v<T>) {
			// void*, push ptr as light user data
			lua_pushlightuserdata(ls, ptr);
		}
		else if constexpr (isLightweight<T>) {
			// lightweight type, push ptr as light user data
			lua_pushlightuserdata(ls, ptr);
#if TY_LUABIND_TYPE_SAFE
			detail::registerTemporaryPointer(ls, ptr);
#endif
		}
		else {
			// Embed ptr into full user data

			const TypeId typeId = getTypeId<T>();
			const lua_Integer ptrKey = detail::makePointerKey(ptr, typeId);

			// Get userdata/table associated with the pointer from registry
			lua_pushinteger(ls, ptrKey);
			lua_rawget(ls, LUA_REGISTRYINDEX);
			if (lua_isnil(ls, -1)) {
				// The ptr is not registered
				lua_pop(ls, 1);

				// Lookup class metatable in registry
				const TypeName typeName = typeIdToName(typeId);
				if (! typeName) {
					lua_pushnil(ls);
					luaL_error(ls, "Unregistered type");
					return; // class not registered in C++
				}

				void* const userData = lua_newuserdatauv(ls, sizeof ptr, 1);
				// Copy C++ pointer to Lua userdata
				std::memcpy(userData, &ptr, sizeof ptr);
				const int userDataIndex = lua_gettop(ls);

				// Set metatable of user data at index idx
				luaL_getmetatable(ls, typeName);
				assert(lua_istable(ls, -1));
				lua_setmetatable(ls, userDataIndex);

				lua_pushinteger(ls, typeId.value());
				lua_setiuservalue(ls, userDataIndex, 1); // ud.userValue[1] = typeId

				// Cache association ptr -> user data in registry
				// registry[ptr] = userData
				lua_pushinteger(ls, ptrKey);
				lua_pushvalue(ls, userDataIndex);
				lua_rawset(ls, LUA_REGISTRYINDEX);
			}
			else {
				// The ptr has been cached. Return it
			}
		}
	}

	static T* pop(lua_State* ls, int idx) {
		T*        ptr = nullptr;
		const int luaType = lua_type(ls, idx);
		if (luaType == LUA_TLIGHTUSERDATA) {
			ptr = static_cast<T*>(lua_touserdata(ls, idx));
			// TODO Type checking
		}
		else if (luaType == LUA_TUSERDATA) {
			std::memcpy(&ptr, lua_touserdata(ls, idx), sizeof ptr);
			lua_getiuservalue(ls, idx, 1);
			assert(! lua_isnil(ls, -1));
			TypeId ptrTypeId;
			ptrTypeId.impl = reinterpret_cast<const void*>(lua_tointeger(ls, -1));
			if (! detail::compatibleTypes(ls, ptrTypeId, getTypeId<T>())) {
				luaL_argerror(ls, idx, "invalid pointer type");
				ptr = nullptr;
			}
		}
		else if (luaType == LUA_TTABLE) {
			// Pointer as _ptr field of a table
			lua_getfield(ls, idx, "_ptr");
			if (! lua_isuserdata(ls, -1)) {
				lua_pop(ls, 1);
				luaL_error(ls, "cannot retrieve raw pointer to C++ object: invalid _ptr field");
			}
			ptr = static_cast<T*>(lua_touserdata(ls, -1));
			lua_pop(ls, 1);
		}
		else if (luaType == LUA_TNIL) {
			ptr = nullptr;
		}
		return ptr;
	}

private:
	using non_const_ptr = std::remove_const_t<T>*;
};

// enum specialization
template <typename T>
class Wrapper<T, typename std::enable_if_t<std::is_enum_v<T>>> {
public:
	static constexpr int getStackSize() {
		return 1;
	}
	static int match(lua_State* ls, int idx) {
		return lua_isnumber(ls, idx);
	}
	static void pushAsKey(lua_State* ls, T arg) {
		push(ls, arg);
	}
	static void push(lua_State* ls, T arg) {
		lua_pushinteger(ls, static_cast<lua_Integer>(arg));
	}
	static T pop(lua_State* ls, int idx) {
		return static_cast<T>(lua_tointeger(ls, idx));
	}
};

// Wrapper of a stack index
template <>
class Wrapper<StackIndex> {
public:
	static constexpr int getStackSize() {
		return 1;
	}
	static int match([[maybe_unused]] lua_State* ls, [[maybe_unused]] int idx) {
		return true;
	}
	static void pushAsKey(lua_State* ls, StackIndex stackIndex) {
		push(ls, stackIndex);
	}
	static void push(lua_State* ls, StackIndex stackIndex) {
		assert(stackIndex.isValid());
		lua_pushvalue(ls, stackIndex.getIndex());
	}
	static StackIndex pop([[maybe_unused]] lua_State* ls, int idx) {
		return StackIndex { idx };
	}
};

// Reference wrapper
template <>
class Wrapper<Reference> {
public:
	static constexpr int getStackSize() {
		return 1;
	}
	static int match([[maybe_unused]] lua_State* ls, [[maybe_unused]] int idx) {
		return true;
	}
	static void pushAsKey(lua_State* ls, Reference ref) {
		push(ls, ref);
	}
	static void push(lua_State* ls, Reference ref) {
		lua_rawgeti(ls, LUA_REGISTRYINDEX, ref.getValue());
	}
	// pop is forbidden. The user should register references manually
	static Reference pop(lua_State* ls, int index) = delete;
};

template <>
class Wrapper<std::string> {
public:
	static constexpr int getStackSize() {
		return 1;
	}
	static int match(lua_State* ls, int idx) {
		return lua_type(ls, idx) == LUA_TSTRING;
	}
	static void pushAsKey(lua_State* ls, const std::string& arg) {
		push(ls, arg);
	}
	static void push(lua_State* ls, const std::string& arg) {
		lua_pushstring(ls, arg.c_str());
	}
	static std::string pop(lua_State* ls, int idx) {
		const char* cstr = lua_tostring(ls, idx);
		return std::string { cstr };
	}
};

template <>
class Wrapper<std::string_view> {
public:
	static constexpr int getStackSize() {
		return 1;
	}
	static int match(lua_State* ls, int idx) {
		return lua_type(ls, idx) == LUA_TSTRING;
	}
	static void pushAsKey(lua_State* ls, const std::string_view& arg) {
		push(ls, arg);
	}
	static void push(lua_State* ls, const std::string_view& arg) {
		lua_pushlstring(ls, arg.data(), arg.size());
	}
	static std::string_view pop(lua_State* ls, int idx) {
		const char* cstr = lua_tostring(ls, idx);
		return std::string_view { cstr };
	}
};

//! Const reference wrapper
// Treat as value
template <class T>
class Wrapper<const T&> : public Wrapper<T> {};

//! Reference wrapper
template <class T>
class Wrapper<T&> {
public:
	static constexpr int getStackSize() {
		return 1;
	}
	static int match(lua_State* ls, int idx) {
		return Wrapper<T*>::match(ls, idx);
	}
	static T& pop(lua_State* ls, int idx) {
		T* ptr = Wrapper<T*>::pop(ls, idx);
		assert(ptr);
		return *ptr;
	}
	static void push(lua_State* ls, T& ref) {
		Wrapper<T*>::push(ls, &ref);
	}
	static void pushAsKey(lua_State* ls, T& ref) {
		push(ref);
	}
};

} // namespace Typhoon::LuaBind
