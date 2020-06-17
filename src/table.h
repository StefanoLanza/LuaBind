#pragma once

#include "tableIterator.h"
#include "reference.h"
#include "stackIndex.h"
#include "typeWrapper.h"
#include "value.h"
#include <cassert>
#include <core/uncopyable.h>

struct lua_State;

namespace Typhoon::LUA {

class Table : Uncopyable {
public:
	Table();
	Table(lua_State* ls, StackIndex stackIndex);
	Table(lua_State* ls, Reference ref);
	Table(Table&& table) noexcept;
	~Table();

	Table& operator=(Table&& table) noexcept;

	explicit operator bool() const {
		return IsValid();
	}
	bool IsValid() const {
		return (ls != nullptr) && ref != LUA_NOREF;
	};
	Reference GetReference() const {
		return Reference(ref);
	}

	//! Return the result of the length operator ('#')
	size_t GetLength() const;

	int GetCount() const;

	void SetFunction(const char* name, lua_CFunction f);
	bool GetFunction(const char* functionName);

	template <class KeyType, class ValueType>
	void Set(const KeyType& key, const ValueType& value) {
		assert(IsValid());
		lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
		PushAsKey(ls, key);
		Push(ls, value);
		lua_settable(ls, -3);
		lua_pop(ls, 1); // table
	}

	template <class KeyType, class ValueType>
	void RawSet(const KeyType& key, const ValueType& value) {
		assert(IsValid());
		lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
		PushAsKey(ls, key);
		Push(ls, value);
		lua_rawset(ls, -3);
		lua_pop(ls, 1); // table
	}

	template <class ValueType>
	void RawSeti(int i, const ValueType& value) {
		assert(IsValid());
		lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
		Push(ls, value);
		// table[i] = value
		lua_rawseti(ls, -2, i);
		lua_pop(ls, 1); // table
	}

	bool HasElement(int index) const;

	Value operator[](const char* key) const;
	Value operator[](int key) const;
	Value operator[](unsigned int key) const;
	Value operator[](double key) const;
	Value operator[](void* key) const;

	TableIterator begin() const {
		return TableIterator(ls, ref, 0);
	}
	TableIterator end() const {
		return TableIterator(ls, ref, GetCount());
	}

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

// Table wrapper
template <>
class Wrapper<Table> {
public:
	static constexpr int stackSize = 1;
	static int           Match(lua_State* ls, int idx) {
        return lua_istable(ls, idx);
	}
	static int PushAsKey(lua_State* ls, const Table& table) {
		lua_rawgeti(ls, LUA_REGISTRYINDEX, table.GetReference().GetValue());
		return 1;
	}
	static int Push(lua_State* ls, const Table& table) {
		lua_rawgeti(ls, LUA_REGISTRYINDEX, table.GetReference().GetValue());
		return 1;
	}
	static Table Get(lua_State* ls, int idx) {
		return Table { ls, StackIndex { idx } };
	}
};

} // namespace Typhoon::LUA
