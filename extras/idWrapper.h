#include <LuaBind/typeWrapper.h>
#include <core/id.h>

namespace Typhoon {

template <class Tag, class Impl>
class LuaBind::Wrapper<Id<Tag, Impl>> {
private:
	using TypedId = Id<Tag, Impl>;

public:
	static constexpr int stackSize = 1;

	static int match(lua_State* ls, int idx) {
		return lua_isnumber(ls, idx);
	}
	static void push(lua_State* ls, TypedId id) {
		lua_pushnumber(ls, static_cast<lua_Number>(id.value));
	}
	static TypedId pop(lua_State* ls, int idx) {
		return TypedId { static_cast<Impl>(lua_tonumber(ls, idx)) };
	}
};

} // namespace Typhoon
