#pragma once

#include "config.h"

#include <src/autoBlock.h>
#include <src/object.h>
#include <src/registration.h>
#include <src/table.h>
#include <src/tableIterator.h>
#include <src/typeSafefy.h>
#include <src/value.h>

#include <functional>

struct lua_State;
typedef int (*lua_CFunction)(lua_State* L);

namespace Typhoon::LuaBind {

// Memory statistics
struct MemoryStats {
	size_t allocatedMemory;
	size_t allocationCount;
	size_t freeCount;
	size_t maxAllocatedSize;
};

class Result;
using Logger = std::function<void(const char*)>;

lua_State*         createState(size_t temporaryCapacity);
void               closeState(lua_State* ls);
lua_State*         getLuaState();
Result             doCommand(lua_State*, const char* command);
Result             doBuffer(lua_State*, const char* buffer, size_t size, const char* name);
void               updateFrame();
void               registerLoader(lua_State* ls, lua_CFunction loader, void* userData);
void               registerLogger(lua_State* ls, Logger&& logger);
const char*        getPath(lua_State* ls);
void               setPath(lua_State* ls, const char* path);
const char*        getErrorMessage(lua_State* ls, int error);
int                getMemoryInUse(lua_State* ls);
const MemoryStats& getMemoryStats();
void*              saveTemporaryPool();
void               restoreTemporaryPool(void* offset);

template <class T>
T* newTemporary() {
	void* mem = detail::allocTemporary(sizeof(T), std::alignment_of_v<T>);
	if (mem) {
		// Construct
		return new (mem) T;
	}
	return nullptr;
}

} // namespace Typhoon::LuaBind

#if TY_LUABIND_NAMESPACE_ALIAS
namespace LuaBind = Typhoon::LuaBind;
#endif
