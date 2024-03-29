#pragma once

#include "table.h"
#include "typeWrapper.h"
#include <vector>

namespace Typhoon::LuaBind {

// Wrapper for std::vector
// Limitations: only support containers of primitive values (those supported by LUA::Value)
template <class T>
class Wrapper<std::vector<T>> {
public:
	static constexpr int stackSize = 1;

	static int match(lua_State* ls, int idx) {
		return lua_istable(ls, idx);
	}

	static void push(lua_State* ls, const std::vector<T>& v) {
		Table table = newTable(ls);

		// Push container values into table
		int idx = 1;
		for (auto it = v.begin(), eit = v.end(); it != eit; ++it, ++idx) {
			table.rawSet(idx, *it);
		}

		// push table on stack
		lua_rawgeti(ls, LUA_REGISTRYINDEX, table.getReference().getValue());
	};

	//\note the container is not cleared
	static std::vector<T> pop(lua_State* ls, int idx) {
		// Open table from stack
		Table table { ls, StackIndex { idx } };
		if (! table.isValid()) {
			return {};
		}

		// Read elements from table and push them in container
		int            i = 1;
		std::vector<T> v;
		for (;;) {
			Value value = table[i];
			if (value) {
				T elm;
				if (value.cast(elm)) {
					v.insert(v.end(), elm);
				}
			}
			else {
				break; // nil
			}
			++i;
		}

		return v;
	}
};

} // namespace Typhoon::LuaBind
