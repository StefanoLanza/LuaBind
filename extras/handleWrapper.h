#include <LuaBind/typeWrapper.h>
#include <core/handle.h>

namespace Typhoon {

template <>
class LuaBind::Wrapper<Handle> {
public:
	static constexpr int stackSize = 1;
	
	static int  match(lua_State* ls, int idx) {
		return lua_isnumber(ls, idx) || lua_isnone(ls, idx);
	}

	static int push(lua_State* ls, Handle handle) {
		if (handle.m_value) {
			// Valid
			lua_pushnumber(ls, static_cast<lua_Number>(handle.m_value));
			return 1;
		}
		else {
			// Nil
			return 0;
		}
	}
	
	static Handle pop(lua_State* ls, int idx) {
		Handle h;
		if (lua_isnumber(ls, idx)) {
			lua_Number luaNumber = lua_tonumber(ls, idx);
			if (luaNumber >= 0 && luaNumber <= std::numeric_limits<Handle::Type>::max()) {
				h.m_value = static_cast<decltype(h.m_value)>(luaNumber);
			}
			else {
				// invalid cast
			}
		}
		else {
			// None; handle remains invalid
		}
		return h;
	}
};

} // namespace Typhoon
