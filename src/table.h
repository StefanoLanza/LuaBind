#pragma once

#include "reference.h"
#include "stackIndex.h"
#include "stackUtils.h"
#include "tableIterator.h"
#include "value.h"
#include <cassert>
#include <core/uncopyable.h>

struct lua_State;

namespace Typhoon::LuaBind {

class Table : Uncopyable {
public:
	Table();
	Table(lua_State* ls, StackIndex stackIndex);
	Table(lua_State* ls, Reference ref);
	Table(Table&& table) noexcept;
	~Table();

	Table& operator=(Table&& table) noexcept;

	explicit operator bool() const {
		return isValid();
	}
	bool isValid() const {
		return (ls != nullptr) && ref != LUA_NOREF;
	};
	Reference getReference() const {
		return Reference(ref);
	}

	//! Return the result of the length operator ('#')
	size_t getLength() const;

	int getCount() const;

	void setFunction(const char* name, lua_CFunction f);
	bool getFunction(const char* functionName);

	template <class KeyType, class ValueType>
	void set(const KeyType& key, const ValueType& value);

	template <class KeyType, class ValueType>
	void rawSet(const KeyType& key, const ValueType& value);

	template <class ValueType>
	void rawSeti(int i, const ValueType& value);

	bool hasElement(int index) const;

	Value operator[](const char* key) const;
	Value operator[](int key) const;
	Value operator[](unsigned int key) const;
	Value operator[](double key) const;
	Value operator[](void* key) const;

	TableIterator begin() const;
	TableIterator end() const;

private:
	lua_State* ls;
	int        ref;
};

//! Return the global environment table
Table globals(lua_State* ls);

//! Return the registry table
Table registry(lua_State* ls);

//! Create and return a new table
Table newtable(lua_State* ls);

} // namespace Typhoon::LuaBind

#include "table.inl"
