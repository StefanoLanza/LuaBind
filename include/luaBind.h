#pragma once

#include "config.h"

#include <src/autoBlock.h>
#include <src/object.h>
#include <src/registration.h>
#include <src/table.h>
#include <src/tableIterator.h>
#include <src/value.h>
#include <src/voidPtrWrapper.h>

struct lua_State;
typedef int (*lua_CFunction)(lua_State* L);

namespace Typhoon {

class Logger;
class Result;

} // namespace Typhoon

namespace Typhoon::LUA {

struct MemoryStats {
	size_t allocatedMemory;
	size_t allocationCount;
	size_t freeCount;
	size_t maxAllocatedSize;
};

lua_State*         createState(size_t temporaryCapacity);
void               closeState(lua_State* ls);
lua_State*         getLuaState();
Result             doCommand(lua_State*, const char* command);
Result             doBuffer(lua_State*, const char* buffer, size_t size, const char* name);
void               updateFrame();
void               registerLoader(lua_State* ls, lua_CFunction loader, void* userData);
void               registerLogger(lua_State* ls, Logger* logger);
const char*        getPath(lua_State* ls);
void               setPath(lua_State* ls, const char* path);
const char*        getErrorMessage(lua_State* ls, int error);
int                getMemoryInUse(lua_State* ls);
const MemoryStats& getMemoryStats();
void* saveTemporaryPool();
void  restoreTemporaryPool(void* offset);

} // namespace Typhoon::LUA