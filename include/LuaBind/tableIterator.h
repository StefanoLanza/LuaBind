#pragma once

#include "reference.h"
#include "value.h"
#include <cassert>
#include <string>

namespace Typhoon::LuaBind {

class Value;

struct TableKeyValue {
	Value key;
	Value value;
};

class TableIterator {
public:
	using difference_type = int;
	using reference = int;

	TableIterator(lua_State* ls, int tableRef, int index);

	TableIterator& operator++();
	TableIterator  operator++(int);
	TableIterator& operator--() { // predecrement
		assert(index >= 0);
		dec();
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
		compat(_Right);
		return (index - _Right.index);
	}

	bool operator==(const TableIterator& _Right) const { // test for iterator equality
		compat(_Right);
		return (index == _Right.index);
	}

	bool operator!=(const TableIterator& _Right) const { // test for iterator inequality
		return (! (*this == _Right));
	}

	bool operator<(const TableIterator& _Right) const { // test if this < _Right
		compat(_Right);
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
	TableKeyValue operator*();

private:
	void compat(const TableIterator& _Right) const;
	void dec();
	bool pushKeyValue() const;

private:
	lua_State* ls;
	int        tableRef;
	int        index;
};

} // namespace Typhoon::LuaBind
