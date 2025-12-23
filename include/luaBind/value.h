#pragma once

#include "reference.h"
#include "stackIndex.h"
#include "typeWrapper.h"
#include <core/typeId.h>
#include <core/uncopyable.h>

#include <optional>
#include <string>

namespace Typhoon::LuaBind {

class Table;

class Value : Uncopyable {
public:
	Value(lua_State* ls, StackIndex stackIndex);
	Value(const Value& value) = delete;
	Value(Value&& value) noexcept;
	~Value();

	Value&       operator=(Value&& value) noexcept;
	const Value& operator=(const Value& value) = delete;

	explicit operator bool() const {
		return ! isNil();
	}
	Reference getReference() const {
		return Reference { ref };
	}
	int                        getType() const;
	bool                       isNil() const;
	std::optional<bool>        asBool() const;
	std::optional<int>         asInt() const;
	std::optional<float>       asFloat() const;
	std::optional<double>      asDouble() const;
	std::optional<const char*> asString() const;
	std::optional<void*>       asPtr() const;
	std::optional<Table>       asTable() const;

	template <class T>
	std::optional<T*> asPtr() const;

	template <class T>
	std::optional<T> as() const;

private:
	bool cast(void*& userData, TypeId typeId) const; // FIXME optional, nullptr is valid

private:
	lua_State* ls;
	int        ref;
};

template <class T>
std::optional<T*> Value::asPtr() const {
	void* userData = nullptr;
	if (! cast(userData, getTypeId<T>())) {
		return std::nullopt;
	}
	return static_cast<T*>(userData);
}

template <class T>
std::optional<T> Value::as() const {
	if constexpr (std::is_same_v<T, bool>) {
		return asBool();
	}
	else if constexpr (std::is_integral_v<T>) {
		return asInt();
	}
	else if constexpr (std::is_floating_point_v<T>) {
		return asDouble();
	}
	else if constexpr (std::is_constructible_v<T, const char*>) {
		return asString();
	}
	else if constexpr (std::is_same_v<std::remove_cv_t<T>, void*>) { // const void* or void*
		return asPtr();
	}
	else if constexpr (std::is_pointer_v<T>) { // typed pointer
		return asPtr<T>();
	}
	else {
		static_assert("Unsupported");
		return std::nullopt;
	}
}

// Value wrapper
template <>
class Wrapper<Value> {
public:
	static constexpr int getStackSize() {
		return 1;
	}
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
