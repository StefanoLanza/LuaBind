#include "tableIterator.h"
#include "autoBlock.h"
#include "value.h"
#include <cassert>

namespace Typhoon::LUA {

TableIterator::TableIterator(lua_State* ls, int tableRef, int index)
    : ls(ls)
    , tableRef(tableRef)
    , index(index) {
}

TableIterator& TableIterator::operator++() {
	++index;
	return (*this);
}

TableIterator TableIterator::operator++(int) {
	TableIterator tmp(*this);
	++index;
	return tmp;
}

Value TableIterator::GetValue() const {
	PushKeyValue();
	// 'key' is at index -2 and 'value' at index -1
	// remove key
	lua_remove(ls, -2);
	// return value
	return Value(ls, topStackIndex);
}

Value TableIterator::GetKey() const {
	PushKeyValue();
	// 'key' is at index -2 and 'value' at index -1
	// remove value
	lua_remove(ls, -1);
	// return key
	return Value(ls, topStackIndex);
}

bool TableIterator::PushKeyValue() const {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, tableRef);
	lua_pushnil(ls); /* first key */
	int i = 0;
	while (lua_next(ls, -2) != 0) {
		// 'key' is at index -2 and 'value' at index -1
		if (i == index) {
			return true;
		}
		// remove 'value'; keep 'key' for next iteration
		lua_pop(ls, 1);
		++i;
	}

	// FIXME Remove key and table?

	assert(0);
	// no more elements
	return false;
}

} // namespace Typhoon::LUA
