#pragma once

#include <array>

struct lua_State;

namespace Typhoon::LUA {

// Wrapper for std::vector
// Limitations: only support containers of primitive values (those supported by LUA::Value)
template <typename T, size_t Size>
class Wrapper<std::array<T, Size>> {
	using ArrayType = std::array<T, Size>;

public:
	static constexpr int stackSize = 1;

	static int Match(lua_State* ls, int idx) {
		return lua_istable(ls, idx);
	}

	static int Push(lua_State* ls, const ArrayType& v) {
		// Create new table
		Table table = newtable(ls);

		// Push container values into table
		int index = 1;
		for (size_t i = 0; i < v.size(); ++i, ++index) {
			table.RawSet(index, v[i]);
		}

		// Push table on stack
		return LUA::Push(ls, table);
	};

	//\note the container is not cleared
	static ArrayType Get(lua_State* ls, int idx) {
		ArrayType array;

		// Open table from stack
		const Table table(ls, StackIndex { idx });
		if (! table.IsValid()) {
			return array;
		}

		// Read elements from table and push them in container
		int tableIndex = 1;
		for (size_t i = 0; i < array.size(); ++i, ++tableIndex) {
			Value value = table[tableIndex];
			if (value) {
				value.Cast(array[i]);
			}
			else {
				break; // nil
			}
		}

		return array;
	}
};

} // namespace Typhoon::LUA