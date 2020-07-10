#pragma once

#include "nil.h"
#include "reference.h"
#include "stackIndex.h"
#include "typeSafefy.h"
#include <cassert>
#include <lua/src/lua.hpp>
#include <string>

namespace Typhoon::LuaBind {

namespace detail {

template <class T>
T* allocTemporary();

}

// typename = void is used for specializations based on std::enable_if. See the enum specialization
template <class T, typename = void>
class Wrapper {
public:
	static int Match(lua_State* ls, int idx) {
		static_assert(false, "Not implemented. Specialize Wrapper for this type.");
		return 0;
	}
	static int PushAsKey(lua_State* ls, T) {
		static_assert(false, "Not implemented. Specialize Wrapper for this type.");
		return 0;
	}
	static int Push(lua_State* ls, T) {
		static_assert(false, "Not implemented. Specialize Wrapper for this type.");
		return 0;
	}
	static T Get(lua_State* ls, int idx) {
		static_assert(false, "Not implemented. Specialize Wrapper for this type.");
		return {};
	}
};

template <>
class Wrapper<Nil> {
public:
	static int Match(lua_State* ls, int idx) {
		return lua_isnil(ls, idx);
	}
	static int Push(lua_State* ls, Nil) {
		lua_pushnil(ls);
		return 1;
	}
	// Cannot get nil
	static constexpr int stackSize = 1;
};

template <>
class Wrapper<char> {
public:
	static int Match(lua_State* ls, int idx) {
		return lua_isnumber(ls, idx);
	}
	static int PushAsKey(lua_State* ls, char arg) {
		return Push(ls, arg);
	}
	static int Push(lua_State* ls, char arg) {
		lua_pushinteger(ls, static_cast<lua_Integer>(arg));
		return 1;
	}
	static unsigned char Get(lua_State* ls, int idx) {
		return static_cast<char>(lua_tointeger(ls, idx));
	}
	static constexpr int stackSize = 1;
};

template <>
class Wrapper<unsigned char> {
public:
	static int Match(lua_State* ls, int idx) {
		return lua_isnumber(ls, idx);
	}
	static int PushAsKey(lua_State* ls, unsigned char arg) {
		return Push(ls, arg);
	}
	static int Push(lua_State* ls, unsigned char arg) {
		lua_pushinteger(ls, static_cast<lua_Integer>(arg));
		return 1;
	}
	static unsigned char Get(lua_State* ls, int idx) {
		return static_cast<unsigned char>(lua_tointeger(ls, idx));
	}
	static constexpr int stackSize = 1;
};

template <>
class Wrapper<int> {
public:
	static int Match(lua_State* ls, int idx) {
		return lua_isnumber(ls, idx);
	}
	static int PushAsKey(lua_State* ls, int arg) {
		lua_pushinteger(ls, static_cast<lua_Integer>(arg));
		return 1;
	}
	static int Push(lua_State* ls, int arg) {
		lua_pushinteger(ls, static_cast<lua_Integer>(arg));
		return 1;
	}
	static int Get(lua_State* ls, int idx) {
		return static_cast<int>(lua_tointeger(ls, idx));
	}
	static constexpr int stackSize = 1;
};

template <>
class Wrapper<unsigned int> {
public:
	static int Match(lua_State* ls, int idx) {
		return lua_isnumber(ls, idx);
	}
	static int PushAsKey(lua_State* ls, unsigned int arg) {
		return Push(ls, arg);
	}
	static int Push(lua_State* ls, unsigned int arg) {
		lua_pushinteger(ls, static_cast<lua_Integer>(arg));
		return 1;
	}
	static unsigned int Get(lua_State* ls, int idx) {
		return static_cast<unsigned int>(lua_tointeger(ls, idx));
	}
	static constexpr int stackSize = 1;
};

template <>
class Wrapper<long> {
public:
	static int Match(lua_State* ls, int idx) {
		return lua_isnumber(ls, idx);
	}
	static int PushAsKey(lua_State* ls, long arg) {
		return Push(ls, arg);
	}
	static int Push(lua_State* ls, long arg) {
		lua_pushinteger(ls, static_cast<lua_Integer>(arg));
		return 1;
	}
	static long Get(lua_State* ls, int idx) {
		return static_cast<long>(lua_tointeger(ls, idx));
	}
	static constexpr int stackSize = 1;
};

template <>
class Wrapper<unsigned long> {
public:
	static int Match(lua_State* ls, int idx) {
		return lua_isnumber(ls, idx);
	}
	static int PushAsKey(lua_State* ls, unsigned long arg) {
		return Push(ls, arg);
	}
	static int Push(lua_State* ls, unsigned long arg) {
		lua_pushinteger(ls, static_cast<lua_Integer>(arg));
		return 1;
	}
	static unsigned long Get(lua_State* ls, int idx) {
		return static_cast<unsigned long>(lua_tointeger(ls, idx));
	}
	static constexpr int stackSize = 1;
};

template <>
class Wrapper<long long> {
public:
	static int Match(lua_State* ls, int idx) {
		return lua_isnumber(ls, idx);
	}
	static int PushAsKey(lua_State* ls, long long arg) {
		return Push(ls, arg);
	}
	static int Push(lua_State* ls, long long arg) {
		lua_pushinteger(ls, static_cast<lua_Integer>(arg));
		return 1;
	}
	static long long Get(lua_State* ls, int idx) {
		return static_cast<long long>(lua_tointeger(ls, idx));
	}
	static constexpr int stackSize = 1;
};

template <>
class Wrapper<unsigned long long> {
public:
	static int Match(lua_State* ls, int idx) {
		return lua_isnumber(ls, idx);
	}
	static int PushAsKey(lua_State* ls, unsigned long long arg) {
		return Push(ls, arg);
	}
	static int Push(lua_State* ls, unsigned long long arg) {
		lua_pushinteger(ls, static_cast<lua_Integer>(arg));
		return 1;
	}
	static unsigned long long Get(lua_State* ls, int idx) {
		return static_cast<unsigned long long>(lua_tointeger(ls, idx));
	}
	static constexpr int stackSize = 1;
};

template <>
class Wrapper<double> {
public:
	static int Match(lua_State* ls, int idx) {
		return lua_isnumber(ls, idx);
	}
	static int PushAsKey(lua_State* ls, double arg) {
		return Push(ls, arg);
	}
	static int Push(lua_State* ls, double arg) {
		lua_pushnumber(ls, static_cast<lua_Number>(arg));
		return 1;
	}
	static double Get(lua_State* ls, int idx) {
		return static_cast<double>(lua_tonumber(ls, idx));
	}
	static constexpr int stackSize = 1;
};

template <>
class Wrapper<float> {
public:
	static constexpr int stackSize = 1;
	static int           Match(lua_State* ls, int idx) {
        return lua_isnumber(ls, idx);
	}
	static int PushAsKey(lua_State* ls, float arg) {
		return Push(ls, arg);
	}
	static int Push(lua_State* ls, float arg) {
		lua_pushnumber(ls, static_cast<lua_Number>(arg));
		return 1;
	}
	static float Get(lua_State* ls, int idx) {
		return static_cast<float>(lua_tonumber(ls, idx));
	}
};

template <>
class Wrapper<bool> {
public:
	static int Match(lua_State* ls, int idx) {
		return lua_isboolean(ls, idx);
	}
	static int Push(lua_State* ls, bool arg) {
		lua_pushboolean(ls, arg ? 1 : 0);
		return 1;
	}
	static bool Get(lua_State* ls, int idx) {
		return lua_toboolean(ls, idx) ? true : false;
	}
	static constexpr int stackSize = 1;
};

template <>
class Wrapper<const char*> {
public:
	static int Match(lua_State* ls, int idx) {
		return lua_type(ls, idx) == LUA_TSTRING;
	}
	static int PushAsKey(lua_State* ls, const char* arg) {
		lua_pushstring(ls, arg);
		return 1;
	}
	static int Push(lua_State* ls, const char* arg) {
		lua_pushstring(ls, arg);
		return 1;
	}
	static const char* Get(lua_State* ls, int idx) {
		return lua_tostring(ls, idx);
	}
	static constexpr int stackSize = 1;
};

//! Raw pointer wrapper
template <class T>
class Wrapper<T*> {
public:
	static int Match(lua_State* ls, int idx) {
		const int luaType = lua_type(ls, idx);
		return (luaType == LUA_TLIGHTUSERDATA || luaType == LUA_TUSERDATA || luaType == LUA_TTABLE);
	}

	static int PushAsKey(lua_State* ls, T* ptr) {
		lua_pushlightuserdata(ls, ptr);
		return 1;
	}

	static int Push(lua_State* ls, T* ptr) {
		if (ptr) {
			// Remove const from pointer type
			using non_const_ptr = std::remove_const_t<T>*;
			void* const ud = const_cast<non_const_ptr>(ptr);

			// Get userdata/table associated with the pointer from registry
			lua_pushlightuserdata(ls, ud);
			lua_rawget(ls, LUA_REGISTRYINDEX);
			if (lua_isnil(ls, -1)) {
				// The ptr is not registered
				// Return it as light user data
				lua_pop(ls, 1);
				lua_pushlightuserdata(ls, ud);
			}
			else {
				// The ptr has been registered. Return it
			}
		}
		else {
			lua_pushnil(ls);
		}
		return 1;
	}

	static T* Get(lua_State* ls, int idx) {
		T*     ptr = nullptr;
		const int luaType = lua_type(ls, idx);
		if (luaType == LUA_TLIGHTUSERDATA) {
			// Light user data
			ptr = static_cast<T*>(lua_touserdata(ls, idx));
		}
		else if (luaType == LUA_TUSERDATA) {
			std::memcpy(&ptr, lua_touserdata(ls, idx), sizeof ptr);
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

#if TY_LUABIND_TYPE_SAFE
		if (ptr && ! detail::checkPointerType(ptr, getTypeId(ptr))) {
			luaL_argerror(ls, idx, "invalid pointer type");
			ptr = nullptr;
		}
#endif
		return ptr;
	}

	static constexpr int stackSize = 1;
};

//! Reference wrapper
template <class T>
class Wrapper<T&> {
public:
	static constexpr int stackSize = 1;
	static int           Match(lua_State* ls, int idx) {
        return Wrapper<T*>::Match(ls, idx);
	}
	static T& Get(lua_State* ls, int idx) {
		T* ptr = Wrapper<T*>::Get(ls, idx);
		assert(ptr);
		return *ptr;
	}
};

//! Const reference wrapper
template <class T>
class Wrapper<const T&> {
public:
	static constexpr int stackSize = 1;
	static int           Match(lua_State* ls, int idx) {
        return Wrapper<const T*>::Match(ls, idx);
	}
	static const T& Get(lua_State* ls, int idx) {
		// Fetch pointer and convert to const reference
		return *Wrapper<const T*>::Get(ls, idx);
	}
	static int Push(lua_State* ls, const T& ref) {
		// Push a copy
		return Wrapper<T>::Push(ls, ref);
	}
};

// enum specialization
template <typename T>
class Wrapper<T, typename std::enable_if_t<std::is_enum_v<T>>> {
public:
	static int Match(lua_State* ls, int idx) {
		return lua_isnumber(ls, idx);
	}
	static int PushAsKey(lua_State* ls, T arg) {
		return Push(ls, arg);
	}
	static int Push(lua_State* ls, T arg) {
		lua_pushinteger(ls, static_cast<lua_Integer>(arg));
		return 1;
	}
	static T Get(lua_State* ls, int idx) {
		return static_cast<T>(lua_tointeger(ls, idx));
	}
	static constexpr int stackSize = 1;
};

// Wrapper of a stack index
template <>
class Wrapper<StackIndex> {
public:
	static int Match(lua_State* /*ls*/, int /*idx*/) {
		return true;
	}
	static int PushAsKey(lua_State* ls, StackIndex stackIndex) {
		return Push(ls, stackIndex);
	}
	static int Push(lua_State* ls, StackIndex stackIndex) {
		assert(stackIndex.isValid());
		lua_pushvalue(ls, stackIndex.getIndex());
		return 1;
	}
	static StackIndex Get(lua_State* /*ls*/, int idx) {
		return StackIndex { idx };
	}
	static constexpr int stackSize = 1;
};

// Reference wrapper
template <>
class Wrapper<Reference> {
public:
	static int Match(lua_State* /*ls*/, int /*idx*/) {
		return true;
	}
	static int PushAsKey(lua_State* ls, Reference ref) {
		return Push(ls, ref);
	}
	static int Push(lua_State* ls, Reference ref) {
		if (ref.isValid()) {
			lua_rawgeti(ls, LUA_REGISTRYINDEX, ref.getValue());
		}
		else {
			lua_pushnil(ls);
		}
		return 1;
	}
	static Reference Get(lua_State* ls, int idx) {
		// Return a reference to the element on the stack
		lua_pushvalue(ls, idx);
		return Reference { luaL_ref(ls, LUA_REGISTRYINDEX) };
	}
	static constexpr int stackSize = 1;
};

template <>
class Wrapper<std::string> {
public:
	static int Match(lua_State* ls, int idx) {
		return lua_type(ls, idx) == LUA_TSTRING;
	}
	static int PushAsKey(lua_State* ls, const std::string& arg) {
		return Push(ls, arg);
	}
	static int Push(lua_State* ls, const std::string& arg) {
		lua_pushstring(ls, arg.c_str());
		return 1;
	}
	static std::string Get(lua_State* ls, int idx) {
		const char* cstr = lua_tostring(ls, idx);
		return std::string { cstr };
	}
	static constexpr int stackSize = 1;
};

// Helper to push and pop temporary objects as light user data
template <class T>
struct Temporary {
	static constexpr int stackSize = 1;
	static int           Match(lua_State* ls, int idx) {
        return lua_isuserdata(ls, idx);
	}
	static int Push(lua_State* ls, const T& value) {
		T* ud = new (detail::allocTemporary<T>()) T { value };
#if TY_LUABIND_TYPE_SAFE
		detail::registerPointerType(ud);
#endif
		lua_pushlightuserdata(ls, ud);
		return 1;
	}
	static T Get(lua_State* ls, int idx) {
		void* const userData = lua_touserdata(ls, idx);
#if TY_LUABIND_TYPE_SAFE
		if (! detail::checkPointerType<T>(userData)) {
			luaL_argerror(ls, idx, "Invalid pointer type"); // TODO better message
		}
#endif
		return *static_cast<const T*>(userData);
	}
};

} // namespace Typhoon::LuaBind
