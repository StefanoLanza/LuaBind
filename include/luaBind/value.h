#pragma once

#include "reference.h"
#include "stackIndex.h"
#include "typeWrapper.h"
#include <core/typeId.h>
#include <core/uncopyable.h>
#include <string>

namespace Typhoon::LuaBind {

class Table;

class Value : Uncopyable {
public:
	Value(lua_State* ls, StackIndex stackIndex);
	Value(Value&& value) noexcept;
	~Value();

	Value& operator=(Value&& value) noexcept;

	explicit operator bool() const {
		return ! isNil();
	}
	Reference getReference() const {
		return Reference { ref };
	}
	int  getType() const;
	bool isNil() const;
	bool cast(int& value) const;
	bool cast(float& value) const;
	bool cast(double& value) const;
	bool cast(bool& value) const;
	bool cast(std::string& value) const;
	bool cast(const char*& value) const;
	bool cast(void*& ptr) const;
	bool cast(Table& table) const;

	template <class T>
	explicit operator T() const {
		T v {};
		cast(v);
		return v;
	}

	// Type safe cast to pointer
	template <class T>
	explicit operator T*() const {
		void* userData = nullptr;
		cast(userData, getTypeId<T>());
		return static_cast<T*>(userData);
	}

	// Not type safe
	explicit operator void*() const {
		void* userData = nullptr;
		cast(userData);
		return userData;
	}

	explicit operator const char*() const {
		const char* str = nullptr;
		cast(str);
		return str;
	}

private:
	bool cast(void*& userData, TypeId typeId) const;

private:
	lua_State* ls;
	int        ref;
};

// Value wrapper
template <>
class Wrapper<Value> {
public:
	static constexpr int stackSize = 1;

	static int match([[maybe_unused]] lua_State* ls, [[maybe_unused]] int idx) {
        return true;
	}
	static void pushAsKey(lua_State* ls, const Value& value) {
		push(ls, value);
	}
	static void push(lua_State* ls, const Value& value) {
		lua_rawgeti(ls, LUA_REGISTRYINDEX, value.getReference().getValue());
	}
	static Value pop(lua_State* ls, int idx) {
		return Value { ls, StackIndex { idx } };
	}
};

} // namespace Typhoon::LuaBind
