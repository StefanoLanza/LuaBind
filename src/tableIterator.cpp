#include "tableIterator.h"
#include "autoBlock.h"
#include "value.h"
#include <cassert>

namespace Typhoon::LuaBind {

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

TableKeyValue TableIterator::operator*() {
	pushKeyValue();
	TableKeyValue kv = { Value(ls, StackIndex(-2)), Value(ls, StackIndex(-1)) };
	lua_pop(ls, 2); // remove key and value from stack
	return kv;
}

void TableIterator::compat(const TableIterator& _Right) const {
	(void)_Right;
	assert(tableRef == _Right.tableRef);
}

void TableIterator::dec() {
	--index;
}

bool TableIterator::pushKeyValue() const {
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

} // namespace Typhoon::LuaBind
