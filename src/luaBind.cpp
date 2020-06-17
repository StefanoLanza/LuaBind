#include "luaBind.h"
#include "autoBlock.h"
#include "debug.h"
#include "private.h"
#include "table.h"
#include <algorithm>
#include <core/heapAllocator.h>
#include <core/linearAllocator.h>
#include <core/result.h>
//#include <logging/logger.h>

namespace Typhoon::LUA {

namespace {

lua_State*  g_ls = nullptr;
MemoryStats memoryStats {};

void* allocFunction(void* ud, void* ptr, size_t osize, size_t nsize) {
	MemoryStats* const stats = reinterpret_cast<MemoryStats*>(ud);
	stats->allocatedMemory -= osize;
	void* pret = nullptr;
	if (nsize == 0) {
		++stats->freeCount;
		free(ptr);
	}
	else {
		++stats->allocationCount;
		stats->allocatedMemory += nsize;
		stats->maxAllocatedSize = std::max(stats->maxAllocatedSize, nsize);
		pret = realloc(ptr, nsize);
	}
	return pret;
}

int logFunction(lua_State* ls) {
	// TODO Remove this dependency
	Logger* logger = static_cast<Logger*>(lua_touserdata(ls, lua_upvalueindex(1)));
	if (lua_isstring(ls, 1)) {
		// LogInfo(*logger, lua_tostring(ls, 1));
	}
	return 0;
}

int panicFunction(lua_State* ls) {
	const char* err = lua_tostring(ls, -1);
	return 0;
}

HeapAllocator heapAllocator;

} // namespace

namespace detail {

Allocator*                       boxedAllocator = nullptr;
std::unique_ptr<LinearAllocator> temporaryAllocator;

} // namespace detail

lua_State* createState(size_t temporaryCapacity) {
	lua_State* ls = lua_newstate(allocFunction, &memoryStats);
	luaL_openlibs(ls);

	// Set our handling function for when Lua panics
	lua_atpanic(ls, &panicFunction);

	AutoBlock autoBlock(ls);
	Table     g = globals(ls);
	g.SetFunction("RegisterLuaClass", detail::registerLuaClass);
	g.SetFunction("GetClassMetatable", detail::getClassMetatable);

	detail::boxedAllocator = &heapAllocator;
	detail::temporaryAllocator = std::make_unique<LinearAllocator>(heapAllocator, temporaryCapacity);
	g_ls = ls;
	return ls;
}

void closeState(lua_State* ls) {
	// Clean up lua
	lua_gc(ls, LUA_GCCOLLECT, 0); // collect garbage
	lua_close(ls);
	detail::temporaryAllocator.reset();
	detail::boxedAllocator = nullptr;
	g_ls = nullptr;
}

lua_State* getLuaState() {
	return g_ls;
}

void updateFrame() {
	detail::temporaryAllocator->Rewind();
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

void registerLogger(lua_State* ls, Logger* logger) {
	lua_rawgeti(ls, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
	lua_pushlightuserdata(ls, logger);
	lua_pushcclosure(ls, logFunction, 1);
	lua_setfield(ls, -2, "log");
	lua_pop(ls, 1); // table
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
	case LUA_ERRGCMM:
		errMsg = "GC error";
		break;
	default:
		break;
	}
	return errMsg;
}

int getMemoryInUse(lua_State* ls) {
	return lua_gc(ls, LUA_GCCOUNT, 0);
}

const MemoryStats& getMemoryStats() {
	return memoryStats;
}

Result doCommand(lua_State* ls, const char* command) {
	return doBuffer(ls, command, strlen(command), nullptr);
}

Result doBuffer(lua_State* ls, const char* buffer, size_t size, const char* name) {
	AutoBlock autoBlock(ls);

	// Put the traceback function on the stack
	lua_pushcfunction(ls, &traceback);
	int errfunc_index = lua_gettop(ls);

	const char* errMsg = getErrorMessage(ls, luaL_loadbuffer(ls, buffer, size, name));
	if (errMsg) {
		return Result(errMsg);
	}
	else if (0 != lua_pcall(ls, 0, 0, errfunc_index)) {
		errMsg = lua_tostring(ls, -1);
		return Result(errMsg);
	}
	else {
		return Result(true);
	}
}

void* saveTemporaryPool() {
	return detail::temporaryAllocator->GetOffset();
}

void restoreTemporaryPool(void* offset) {
	detail::temporaryAllocator->Rewind(offset);
}

} // namespace Typhoon::LUA
