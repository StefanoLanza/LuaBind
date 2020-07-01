#include "registration.h"
#include <cassert>

namespace Typhoon::LuaBind::detail {

int beginNamespace(lua_State* ls, const char* name) {
	assert(name);
	Table namespaceTable = newtable(ls);
	assert(! globals(ls)[name]);
	globals(ls).set(name, namespaceTable);
	lua_rawgeti(ls, LUA_REGISTRYINDEX, namespaceTable.getReference().getValue());
	return lua_gettop(ls);
}

} // namespace Typhoon::LuaBind::detail
