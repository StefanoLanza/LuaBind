#pragma once

#include "reference.h"
#include "stackIndex.h"
#include "typeWrapper.h"
#include <core/typeId.h>
#include <core/uncopyable.h>

#include <optional>

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
	std::optional<Nil>         asNil() const;
	std::optional<bool>        asBool() const;
	std::optional<lua_Integer> asInteger() const;
	std::optional<lua_Number>  asNumber() const;
	std::optional<const char*> asString() const;
	std::optional<void*>       asUserData() const;
	std::optional<Table>       asTable() const;

	template <class T>
	std::optional<T*> asPtr() const;

	template <class T>
	std::optional<T> as() const;

private:
	std::optional<void*> toPtr(TypeId typeId) const;

private:
	lua_State* ls;
	int        ref;
};

template <class T>
std::optional<T*> Value::asPtr() const {
	if (auto res = toPtr(getTypeId<T>())) {
		return static_cast<T*>(res.value());
	}
	return std::nullopt;
}

template <class T>
std::optional<T> Value::as() const {
	if constexpr (std::is_same_v<T, Nil>) { 
		return asNil();
	}
	else if constexpr (std::is_same_v<T, bool>) {
		return asBool();
	}
	else if constexpr (std::is_integral_v<T>) {
		if (auto res = asInteger(); res) {
			return static_cast<T>(res.value());
		}
		return std::nullopt;
	}
	else if constexpr (std::is_floating_point_v<T>) {
		if (auto res = asNumber(); res) {
			return static_cast<T>(res.value());
		}
		return std::nullopt;
	}
	else if constexpr (std::is_constructible_v<T, const char*>) {
		return asString();
	}
	else if constexpr (std::is_same_v<std::remove_cv_t<T>, void*>) { // const void* or void*
		return asUserData();
	}
	else if constexpr (std::is_pointer_v<T>) { // typed pointer
		return asPtr<T>();
	}
	else if constexpr (std::is_same_v<T, Table>) { 
		return asTable();
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
