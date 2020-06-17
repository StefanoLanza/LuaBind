#pragma once

#include "reference.h"
#include "stackIndex.h"
#include "typeWrapper.h"
#include <core/typeId.h>
#include <core/uncopyable.h>
#include <string>

struct lua_State;

namespace Typhoon::LUA {

class Table;

class Value : Uncopyable {
public:
	Value(lua_State* ls, StackIndex stackIndex);
	Value(Value&& value) noexcept;
	~Value();

	Value& operator=(Value&& value) noexcept;

	explicit operator bool() const {
		return ! IsNil();
	}
	Reference GetReference() const {
		return Reference { ref };
	}
	int  getType() const;
	bool IsNil() const;
	bool Cast(int& value) const;
	bool Cast(float& value) const;
	bool Cast(double& value) const;
	bool Cast(bool& value) const;
	bool Cast(std::string& value) const;
	bool Cast(const char*& value) const;
	bool Cast(void*& ptr) const;
	bool Cast(Table& table) const;

	template <class T>
	explicit operator T() const {
		T v;
		Cast(v);
		return v;
	}

	// Type safe cast to pointer
	template <class T>
	explicit operator T*() const {
		void* userData = nullptr;
		Cast(userData, getTypeId<T>());
		return static_cast<T*>(userData);
	}

	// Not type safe
	explicit operator void*() const {
		void* userData = nullptr;
		Cast(userData);
		return userData;
	}

	explicit operator const char*() const {
		const char* str = nullptr;
		Cast(str);
		return str;
	}

private:
	bool Cast(void*& userData, TypeId typeId) const;

private:
	lua_State* ls;
	int        ref;
};

// Value wrapper
template <>
class Wrapper<Value> {
public:
	static constexpr int stackSize = 1;
	static int           Match(lua_State* /*ls*/, int /*idx*/) {
        return true;
	}
	static int PushAsKey(lua_State* ls, const Value& value) {
		lua_rawgeti(ls, LUA_REGISTRYINDEX, value.GetReference().GetValue());
		return 1;
	}
	static int Push(lua_State* ls, const Value& value) {
		lua_rawgeti(ls, LUA_REGISTRYINDEX, value.GetReference().GetValue());
		return 1;
	}
	static Value Get(lua_State* ls, int idx) {
		return Value { ls, StackIndex { idx } };
	}
};

} // namespace Typhoon::LUA
