#include "luaBind.h"
#include "autoBlock.h"
#include "context.h"
#include "debug.h"
#include "detail.h"
#include "result.h"
#include "table.h"

#include <algorithm>
#include <core/scopedAllocator.h>

namespace Typhoon::LuaBind {

namespace {

const char* contextKey = "__context";

void* allocFunction(void* ud, void* ptr, size_t osize, size_t nsize) {
	Allocator*   allocator = static_cast<Context*>(ud)->allocator;
	MemoryStats& stats = static_cast<Context*>(ud)->memoryStats;
	stats.allocatedMemory -= osize;
	void* pret = nullptr;
	if (nsize == 0) {
		++stats.freeCount;
		allocator->free(ptr, osize);
	}
	else {
		++stats.allocationCount;
		stats.allocatedMemory += nsize;
		stats.maxAllocatedSize = std::max(stats.maxAllocatedSize, nsize);
		pret = allocator->realloc(ptr, osize, nsize, Allocator::defaultAlignment);
	}
	return pret;
}

void luaWarningFunction(void* ud, const char* msg, [[maybe_unused]] int tocont) {
	auto const& warningFunction = static_cast<Context*>(ud)->warningFunction;
	warningFunction(msg);
}

} // namespace

namespace detail {

	Context* getContext(lua_State* ls) {
	lua_pushvalue(ls, LUA_REGISTRYINDEX);
	lua_getfield(ls, -1, contextKey);
	auto context = static_cast<Context*>(lua_touserdata(ls, -1));
	lua_pop(ls, 2);
	return context;
}

} // namespace detail

lua_State* createState(Allocator& allocator) {
	auto context = allocator.construct<Context>();
	if (! context) {
		return nullptr;
	}
	context->allocator = &allocator;
	lua_State* ls = lua_newstate(allocFunction, context, 0);
	if (! ls) {
		return nullptr;
	}
	context->ls = ls;
	luaL_openlibs(ls);

	AutoBlock autoBlock(ls);
	Table     globals = getGlobals(ls);
	globals.setFunction("RegisterLuaClass", detail::registerLuaClass);
	globals.setFunction("GetClassMetatable", detail::getClassMetatable);

	context->tempAllocator = allocator.construct<PagedAllocator>(allocator, PagedAllocator::defaultPageSize);
	context->scopedAllocator = allocator.construct<ScopedAllocator>(*context->tempAllocator);
	context->currScopedAllocator = context->scopedAllocator;

	// Associate context with ls
	lua_pushvalue(ls, LUA_REGISTRYINDEX);
	lua_pushlightuserdata(ls, context);
	lua_setfield(ls, -2, contextKey);
	lua_pop(ls, 1);

	return ls;
}

void closeState(lua_State* ls) {
	auto context = detail::getContext(ls);
	lua_close(ls);
	if (context->allocator) {
		context->allocator->destroy(context->scopedAllocator);
		context->allocator->destroy(context->tempAllocator);
		context->allocator->destroy(context);
		context->allocator = nullptr;
	}
}

void setWarningFunction(lua_State* ls, WarningFunction warningFunction) {
	auto context = detail::getContext(ls);
	context->warningFunction = std::move(warningFunction);
	lua_setwarnf(ls, luaWarningFunction, context);
}

void resetAllocator(lua_State* ls) {
	auto context = detail::getContext(ls);
	context->scopedAllocator->destroyAll();
#if TY_LUABIND_TYPE_SAFE
	context->tempPointerMap.clear();
#endif
}

void registerLoader(lua_State* ls, lua_CFunction loader, void* userData) {
	lua_getglobal(ls, "package");
	lua_getfield(ls, -1, "searchers");
	lua_remove(ls, -2);
	lua_Integer numLoaders = 0;
	lua_pushnil(ls);
	while (lua_next(ls, -2) != 0) {
		lua_pop(ls, 1);
		++numLoaders;
	}
	lua_pushinteger(ls, numLoaders + 1);
	lua_pushlightuserdata(ls, userData);
	lua_pushcclosure(ls, loader, 1);
	lua_rawset(ls, -3);
	lua_pop(ls, 1); // pop table
}

const char* getPath(lua_State* ls) {
	AutoBlock autoBlock(ls);

	lua_rawgeti(ls, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
	lua_pushstring(ls, "LUA_PATH");
	lua_gettable(ls, -2);

	const char* path = nullptr;
	if (lua_isstring(ls, -1)) {
		path = lua_tostring(ls, -1);
	}
	lua_pop(ls, 1);
	return path;
}

void setPath(lua_State* ls, const char* path) {
	AutoBlock autoBlock(ls);
	lua_getglobal(ls, "package");
	lua_getfield(ls, -1, "path");                // get field "path" from table at top of stack (-1)
	std::string cur_path = lua_tostring(ls, -1); // grab path string from top of stack
	cur_path.push_back(';');                     // do your path magic here
	cur_path.append(path);
	lua_pop(ls, 1);                       // get rid of the string on the stack we just pushed on line 5
	lua_pushstring(ls, cur_path.c_str()); // push the new one
	lua_setfield(ls, -2, "path");         // set the field "path" in table at -2 with value at top of stack
}

const char* getErrorMessage(lua_State* ls, int error) {
	const char* errMsg = nullptr;
	switch (error) {
	case LUA_ERRMEM:
		errMsg = "Out of memory.";
		break;
	case LUA_ERRERR:
		errMsg = "Running error.";
		break;
	case LUA_ERRFILE:
		errMsg = "File not found";
		break;
	case LUA_ERRRUN:
	case LUA_ERRSYNTAX:
		errMsg = lua_tostring(ls, -1);
		break;
	default:
		break;
	}
	return errMsg;
}

int getMemoryInUse(lua_State* ls) {
	return lua_gc(ls, LUA_GCCOUNT, 0);
}

const MemoryStats& getMemoryStats(lua_State* ls) {
	return detail::getContext(ls)->memoryStats;
}

Result doCommand(lua_State* ls, const char* command) {
	return doBuffer(ls, command, strlen(command), nullptr);
}

Result doBuffer(lua_State* ls, const char* buffer, size_t size, const char* name) {
	AutoBlock autoBlock(ls);

	// Put the traceback function on the stack
	lua_pushcfunction(ls, &traceback);
	const int errfunc_index = lua_gettop(ls);

	const auto error = luaL_loadbuffer(ls, buffer, size, name);

	if (const char* errMsg = getErrorMessage(ls, error); errMsg) {
		return UNEXPECTED(errMsg);
	}
	else if (0 != lua_pcall(ls, 0, 0, errfunc_index)) {
		return UNEXPECTED(lua_tostring(ls, -1));
	}
	else {
		return {};
	}
}

Scope::Scope(lua_State* ls)
    : context { detail::getContext(ls) }
    , scopedAllocator { *static_cast<Context*>(context)->tempAllocator } {
	prevAllocator = static_cast<Context*>(context)->currScopedAllocator;
	static_cast<Context*>(context)->currScopedAllocator = &scopedAllocator;
}

Scope::~Scope() {
	static_cast<Context*>(context)->currScopedAllocator = prevAllocator;
}

} // namespace Typhoon::LuaBind
