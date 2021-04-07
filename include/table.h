#pragma once

#include "reference.h"
#include "stackIndex.h"
#include "stackUtils.h"
#include "tableIterator.h"
#include "value.h"
#include <cassert>
#include <core/uncopyable.h>

namespace Typhoon::LuaBind {

class Table : Uncopyable {
public:
	Table();
	Table(lua_State* ls, StackIndex stackIndex);
	Table(lua_State* ls, Reference ref);
	Table(Table&& table) noexcept;
	~Table();

	Table& operator=(Table&& table) noexcept;

	//
	explicit operator bool() const {
		return isValid();
	}

	//
	bool isValid() const;

	//
	Reference getReference() const {
		return Reference(ref);
	}

	//! Return the result of the length operator ('#')
	size_t getLength() const;

	//
	int getCount() const;

	//
	void setFunction(const char* name, lua_CFunction f);

	//
	bool getFunction(const char* functionName);

	//
	template <class KeyType, class ValueType>
	void set(const KeyType& key, const ValueType& value);

	//
	template <class KeyType, class ValueType>
	void rawSet(const KeyType& key, const ValueType& value);

	//
	template <class ValueType>
	void rawSeti(lua_Integer i, const ValueType& value);

	//
	bool hasElement(lua_Integer index) const;

	//
	template <class KeyType>
	Value operator[](const KeyType& key) const;

	//
	Value operator[](const char* key) const;

	//
	Value operator[](Reference reference) const;

	//
	TableIterator begin() const;

	//
	TableIterator end() const;

private:
	lua_State* ls;
	int        ref;
	bool       isRegistry;
};

//! Return the global environment table
Table getGlobals(lua_State* ls);

//! Return the registry table
Table getRegistry(lua_State* ls);

//! Create and return a new table
Table newTable(lua_State* ls);

} // namespace Typhoon::LuaBind

#include "table.inl"
