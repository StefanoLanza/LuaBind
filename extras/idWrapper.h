#include <luaBind/typeWrapper.h>
#include <core/id.h>

namespace Typhoon {

template <class Tag, class Impl>
class LuaBind::Wrapper<Id<Tag, Impl>> {
private:
	using TypedId = Id<Tag, Impl>;

public:
	static constexpr int stackSize = 1;

	static int match(lua_State* ls, int idx) {
		return lua_isinteger(ls, idx);
	}
	static void push(lua_State* ls, TypedId id) {
		if (id.value) {
			lua_pushinteger(ls, static_cast<lua_Integer>(id.value));
		}
		else {
			lua_pushnil(ls);
		}
	}
	static TypedId pop(lua_State* ls, int idx) {
		return TypedId { static_cast<Impl>(lua_tonumber(ls, idx)) };
	}
};

} // namespace Typhoon
