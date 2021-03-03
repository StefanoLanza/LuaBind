#pragma once

#include "config.h"

#include "autoBlock.h"
#include "object.h"
#include "objectRegistration.h"
#include "registration.h"
#include "table.h"
#include "tableIterator.h"
#include "typeSafefy.h"
#include "uniqueRef.h"
#include "value.h"

#include <functional>

namespace Typhoon {
	class Allocator;
}

namespace Typhoon::LuaBind {

// Memory statistics
struct MemoryStats {
	size_t allocatedMemory;
	size_t allocationCount;
	size_t freeCount;
	size_t maxAllocatedSize;
};

class Result;
using Logger = void (*)(const char*, void* ud);

lua_State*         createState(Allocator& allocator);
void               closeState(lua_State* ls);
lua_State*         getLuaState();
Result             doCommand(lua_State*, const char* command);
Result             doBuffer(lua_State*, const char* buffer, size_t size, const char* name);
void               newFrame();
void               registerLoader(lua_State* ls, lua_CFunction loader, void* userData);
void               registerLogger(lua_State* ls, Logger logger, void* ud);
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
