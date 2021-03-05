#include "registration.h"
#include <cassert>

namespace Typhoon::LuaBind::detail {

int beginNamespace(lua_State* ls, const char* name) {
	assert(name);
	assert(! getGlobals(ls)[name]);
	Table namespaceTable = newTable(ls);
	getGlobals(ls).set(name, namespaceTable);
	lua_rawgeti(ls, LUA_REGISTRYINDEX, namespaceTable.getReference().getValue());
	return lua_gettop(ls);
}

} // namespace Typhoon::LuaBind::detail
