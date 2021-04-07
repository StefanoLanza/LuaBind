#pragma once

#include "table.h"
#include "tableWrapper.h"
#include <array>

namespace Typhoon::LuaBind {

// Wrapper for std::vector
// Limitations: only support containers of primitive values (those supported by LUA::Value)
template <typename T, size_t Size>
class Wrapper<std::array<T, Size>> {
	using ArrayType = std::array<T, Size>;

public:
	static constexpr int stackSize = 1;

	static int match(lua_State* ls, int idx) {
		return lua_istable(ls, idx);
	}

	static int push(lua_State* ls, const ArrayType& v) {
		// Create new table
		Table table = newTable(ls);

		// Push container values into table
		int index = 1;
		for (size_t i = 0; i < v.size(); ++i, ++index) {
			table.rawSet(index, v[i]);
		}

		// Push table on stack
		return Wrapper<Table>::push(ls, table);
	};

	//\note the container is not cleared
	static ArrayType pop(lua_State* ls, int idx) {
		ArrayType array;

		// Open table from stack
		const Table table(ls, StackIndex { idx });
		if (! table.isValid()) {
			return array;
		}

		// Read elements from table and push them in container
		int tableIndex = 1;
		for (size_t i = 0; i < array.size(); ++i, ++tableIndex) {
			Value value = table[tableIndex];
			if (value) {
				value.cast(array[i]);
			}
			else {
				break; // nil
			}
		}

		return array;
	}
};

} // namespace Typhoon::LuaBind
