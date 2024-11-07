#include <core/id.h>
#include <luaBind/typeWrapper.h>

namespace Typhoon {

template <class Tag, class Impl, Impl nullValue>
class LuaBind::Wrapper<Id<Tag, Impl, nullValue>> {
private:
	using TypedId = Id<Tag, Impl, nullValue>;

public:
	static constexpr int stackSize = 1;

	// An invalid id becomes nil in Lua

	static int match(lua_State* ls, int idx) {
		return lua_isinteger(ls, idx) | lua_isnil(ls, idx);
	}
	static void push(lua_State* ls, TypedId id) {
		if (id) {
			lua_pushinteger(ls, static_cast<lua_Integer>(id.getValue()));
		}
		else {
			lua_pushnil(ls);
		}
	}
	static TypedId pop(lua_State* ls, int idx) {
		if (lua_isnil(ls, idx)) {
			return TypedId {};
		}
		else {
			return TypedId { static_cast<Impl>(lua_tonumber(ls, idx)) };
		}
	}
};

} // namespace Typhoon
