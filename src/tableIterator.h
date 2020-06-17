#pragma once

#include "reference.h"
#include "value.h"
#include <cassert>
#include <string>

struct lua_State;

namespace Typhoon::LUA {

class Value;

class TableIterator {
public:
	using difference_type = int;
	using reference = int;

	TableIterator(lua_State* ls, int tableRef, int index);

	TableIterator& operator++();
	TableIterator  operator++(int);
	TableIterator& operator--() { // predecrement
		assert(index >= 0);
		Dec();
		return (*this);
	}

	TableIterator operator--(int) { // postdecrement
		TableIterator tmp = *this;
		--*this;
		return (tmp);
	}

	TableIterator& operator+=(int inc) {
		assert(index + inc >= 0);
		index += inc;
		return *this;
	}

	TableIterator& operator-=(int inc) {
		return (*this += -inc);
	}

	TableIterator operator+(int off) const { // return this + integer
		TableIterator tmp = *this;
		return (tmp += off);
	}

	TableIterator operator-(int off) const { // return this + integer
		TableIterator tmp = *this;
		return (tmp -= off);
	}

	//! return difference of iterators
	difference_type operator-(const TableIterator& _Right) const {
		Compat(_Right);
		return (index - _Right.index);
	}

	bool operator==(const TableIterator& _Right) const { // test for iterator equality
		Compat(_Right);
		return (index == _Right.index);
	}

	bool operator!=(const TableIterator& _Right) const { // test for iterator inequality
		return (! (*this == _Right));
	}

	bool operator<(const TableIterator& _Right) const { // test if this < _Right

		Compat(_Right);
		return (index < _Right.index);
	}

	bool operator>(const TableIterator& _Right) const { // test if this > _Right
		return (_Right < *this);
	}

	bool operator<=(const TableIterator& _Right) const { // test if this <= _Right
		return (! (_Right < *this));
	}

	bool operator>=(const TableIterator& _Right) const { // test if this >= _Right
		return (! (*this < _Right));
	}
	Value operator*() {
		return GetValue();
	}
	Value GetValue() const;
	Value GetKey() const;

private:
	void Compat(const TableIterator& _Right) const {
		(void)_Right;
		assert(tableRef == _Right.tableRef);
	}

	void Dec() {
		--index;
	}
	bool PushKeyValue() const;

private:
	lua_State* ls;
	int        tableRef;
	int        index;
};

} // namespace Typhoon::LUA
