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

#include <core/uncopyable.h>

namespace Typhoon {
class Allocator;
class LinearAllocator;
} // namespace Typhoon

namespace Typhoon::LuaBind {

// Memory statistics
struct MemoryStats {
	size_t allocatedMemory;
	size_t allocationCount;
	size_t freeCount;
	size_t maxAllocatedSize;
};

struct Context;
class Result;

using WarningFunction = std::function<void(const char*)>;

lua_State*         createState(Allocator& allocator);
void               closeState(lua_State* ls);
void               setWarningFunction(lua_State* ls, WarningFunction warningFunction);
Result             doCommand(lua_State*, const char* command);
Result             doBuffer(lua_State*, const char* buffer, size_t size, const char* name);
void               resetAllocator(lua_State* ls);
void               registerLoader(lua_State* ls, lua_CFunction loader, void* userData);
const char*        getPath(lua_State* ls);
void               setPath(lua_State* ls, const char* path);
const char*        getErrorMessage(lua_State* ls, int error);
int                getMemoryInUse(lua_State* ls);
const MemoryStats& getMemoryStats(const Context* context);

class Scope : Uncopyable {
public:
	Scope(lua_State* ls);
	~Scope();

private:
	Context*         context;
	ScopedAllocator  allocator;
	ScopedAllocator* prevAllocator;
};

template <class T>
inline Reference makeRef(lua_State* ls, const T& obj) {
	push(ls, obj);
	return Reference { luaL_ref(ls, LUA_REGISTRYINDEX) };
}

Typhoon::ScopedAllocator* getTemporaryAllocator(lua_State* ls);

// Helper
template <class T, class... ArgTypes>
T* allocTemporary(lua_State* ls, ArgTypes&&... args) {
	return getTemporaryAllocator(ls)->make<T>(std::forward<ArgTypes>(args)...);
}

} // namespace Typhoon::LuaBind

#if TY_LUABIND_NAMESPACE_ALIAS
namespace LuaBind = Typhoon::LuaBind;
#endif
