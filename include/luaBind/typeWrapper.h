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

template <class T>
T* allocTemporary(lua_State* ls);

template <typename...>
struct always_false {
	static constexpr bool value = false;
};

} // namespace detail

template <class T, class... ArgTypes>
inline T* newTemporary(lua_State* ls, ArgTypes... args);

// typename = void is used for specializations based on std::enable_if. See the enum specialization
// Generic wrapper
// The object is copied to Lua userdata, or light userdata
template <class T, typename = void>
class Wrapper {
public:
	static constexpr int stackSize = 1;

	static int match(lua_State* ls, int idx) {
		return lua_isuserdata(ls, idx);
	}
	static void push(lua_State* ls, const T& value) {
		// TODO handle lightweight trait here
		// 
		// void* mem = nullptr;
		/* if constexpr (isLightweight<T>) {
		    mem = detail::allocTemporary<T>(ls);
		    T* ptr = new (mem) T { value };
		    Lightweight<T>::push(ls, ptr);
		}
		else*/
		{
			// TODO Remove indirection, store value in ud directly
			const T* valueCopy = new T { value }; // copy value

			// Allocate full user data and store the object pointer in it
			void* const ud = lua_newuserdatauv(ls, sizeof valueCopy, 1);
			std::memcpy(ud, &valueCopy, sizeof valueCopy);			

			// Associate metatable
			const char* className = typeName<T>();
			assert(className);
			luaL_getmetatable(ls, className);
			lua_setmetatable(ls, -2);

			// Mark as heap allocated by Lua. This user value is queried in wrapDefaultDelete<T>
			lua_pushinteger(ls, detail::kLuaAllocated);
			lua_setiuservalue(ls, -2, 1); // ud.userValue[1] = kLuaAllocated

#if TY_LUABIND_TYPE_SAFE
			detail::registerPointer(ls, valueCopy);
#endif
		}
	}
	static T pop(lua_State* ls, int idx) {
		void* userData = lua_touserdata(ls, idx);
		assert(userData);
#if TY_LUABIND_TYPE_SAFE
		if (! detail::checkPointerType<T>(ls, userData)) {
			luaL_argerror(ls, idx, "Invalid pointer type"); // TODO better message
		}
#endif
		return *static_cast<const T*>(userData);
	}
};

template <>
class Wrapper<Nil> {
public:
	static constexpr int stackSize = 1;

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
	static constexpr int stackSize = 1;

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
	static constexpr int stackSize = 1;

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
	static constexpr int stackSize = 1;

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
	static constexpr int stackSize = 1;

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
	static constexpr int stackSize = 1;

	static int match(lua_State* ls, int idx) {
		const int luaType = lua_type(ls, idx);
		return (luaType == LUA_TLIGHTUSERDATA || luaType == LUA_TUSERDATA || luaType == LUA_TTABLE || luaType == LUA_TNIL);
	}

	static void pushAsKey(lua_State* ls, T* ptr) {
		lua_pushlightuserdata(ls, const_cast<non_const_ptr>(ptr));
	}

	static void push(lua_State* ls, T* ptr) {
		if (ptr) {
			// Remove const from pointer type
			void* const ud = const_cast<non_const_ptr>(ptr);

			// Get userdata/table associated with the pointer from registry
			lua_pushlightuserdata(ls, ud);
			lua_rawget(ls, LUA_REGISTRYINDEX);
			if (lua_isnil(ls, -1)) {
				// The ptr is not registered
				// Return it as light user data
				lua_pop(ls, 1);
				lua_pushlightuserdata(ls, ud);

#if TY_LUABIND_TYPE_SAFE
				if constexpr (! std::is_void<T>()) {
					detail::registerPointer(ls, ptr);
				}
				// else void* ptr, type is lost
#endif
			}
			else {
				// The ptr has been registered. Return it
			}
		}
		else {
			lua_pushnil(ls);
		}
	}

	static T* pop(lua_State* ls, int idx) {
		T*        ptr = nullptr;
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
		else if (luaType == LUA_TNIL) {
			ptr = nullptr;
		}

#if TY_LUABIND_TYPE_SAFE
		if (ptr && ! detail::checkPointerType(ls, ptr, getTypeId(ptr))) {
			ptr = nullptr;
			luaL_argerror(ls, idx, "invalid pointer type");
		}
#endif
		return ptr;
	}

private:
	using non_const_ptr = std::remove_const_t<T>*;
};

// enum specialization
template <typename T>
class Wrapper<T, typename std::enable_if_t<std::is_enum_v<T>>> {
public:
	static constexpr int stackSize = 1;

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
	static constexpr int stackSize = 1;

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
	static constexpr int stackSize = 1;

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
};

template <>
class Wrapper<std::string> {
public:
	static constexpr int stackSize = 1;

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
	static constexpr int stackSize = 1;

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

// Helper to push and pop temporary objects as light user data
template <class T>
struct Lightweight {
	static constexpr int stackSize = 1;
	static_assert(std::is_trivially_destructible_v<T>, "Lightweight trait requires a trivially destructible class");

	static int match(lua_State* ls, int idx) {
		return lua_isuserdata(ls, idx);
	}
	static void push(lua_State* ls, const T& value) {
		void* const mem = detail::allocTemporary<T>(ls);
		if (mem) {
			T* ud = new (mem) T { value };
#if TY_LUABIND_TYPE_SAFE
			detail::registerPointer(ls, ud);
#endif
			lua_pushlightuserdata(ls, ud);
		}
		else {
			lua_pushnil(ls);
		}
	}
	static T pop(lua_State* ls, int idx) {
		void* userData = lua_touserdata(ls, idx);
		assert(userData);
#if TY_LUABIND_TYPE_SAFE
		if (! detail::checkPointerType<T>(ls, userData)) {
			luaL_argerror(ls, idx, "Invalid pointer type"); // TODO better message
		}
#endif
		return *static_cast<const T*>(userData);
	}
};

template <class T>
inline constexpr bool isLightweight = std::is_base_of_v<Lightweight<T>, Wrapper<T>>;

//! Const reference wrapper
template <class T>
class Wrapper<const T&> {
	// If Wrapper<T> specialization of Wrapper<T> exists, use that to push and pop
	// Otherwise, treat the reference as a pointer
	static constexpr bool isDefined = (Wrapper<T>::stackSize > 0);
	using PopType = std::conditional_t<isDefined, T, const T&>;

public:
	static constexpr int stackSize = isDefined ? Wrapper<T>::stackSize : 1;

	static int match(lua_State* ls, int idx) {
		if constexpr (isDefined) {
			return Wrapper<T>::match(ls, idx);
		}
		else {
			return Wrapper<const T*>::match(ls, idx);
		}
	}
	static PopType pop(lua_State* ls, int idx) {
		if constexpr (isDefined) {
			// Pop and return value
			return Wrapper<T>::pop(ls, idx);
		}
		else {
			// Pop pointer and convert to const reference
			return *Wrapper<const T*>::pop(ls, idx);
		}
	}
	static void push(lua_State* ls, const T& ref) {
		if constexpr (isDefined) {
			// Push a copy
			Wrapper<T>::push(ls, ref);
		}
		else {
			// Convert to pointer and push
			Wrapper<const T*>::push(ls, &ref);
		}
	}
	static void pushAsKey(lua_State* ls, const T& ref) {
		if constexpr (isDefined) {
			// Push a copy
			Wrapper<T>::pushAsKey(ls, ref);
		}
		else {
			// Convert to pointer and push
			Wrapper<const T*>::pushAsKey(ls, &ref);
		}
	}
};

//! Reference wrapper
template <class T>
class Wrapper<T&> {
public:
	static constexpr int stackSize = 1;

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
