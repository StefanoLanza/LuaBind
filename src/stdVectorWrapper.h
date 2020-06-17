#pragma once

#include <vector>

struct lua_State;

namespace Typhoon::LUA {

// Wrapper for std::vector
// Limitations: only support containers of primitive values (those supported by LUA::Value)
template <class T>
class Wrapper<std::vector<T>> {
public:
	static int Match(lua_State* ls, int idx) {
		return lua_istable(ls, idx);
	}

	static int Push(lua_State* ls, const std::vector<T>& v) {
		// Create new table
		Table table = newtable(ls);

		// Push container values into table
		int idx = 1;
		for (auto it = v.begin(), eit = v.end(); it != eit; ++it, ++idx) {
			table.RawSet(idx, *it);
		}

		// push table on stack
		lua_rawgeti(ls, LUA_REGISTRYINDEX, table.GetReference().GetValue());
		return 1;
	};

	//\note the container is not cleared
	static std::vector<T> Get(lua_State* ls, int idx) {
		// Open table from stack
		Table table { ls, StackIndex { idx } };
		if (! table.IsValid()) {
			return {};
		}

		// Read elements from table and push them in container
		int            i = 1;
		std::vector<T> v;
		for (;;) {
			Value value = table[i];
			if (value) {
				T elm;
				if (value.Cast(elm)) {
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
	static constexpr int stackSize = 1;
};

} // namespace Typhoon::LUA
