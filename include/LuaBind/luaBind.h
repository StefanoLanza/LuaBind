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

namespace Typhoon {
	class Allocator;
	class LinearAllocator;
}

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

lua_State*         createState(Allocator& allocator);
void               closeState(lua_State* ls);
Result             doCommand(lua_State*, const char* command);
Result             doBuffer(lua_State*, const char* buffer, size_t size, const char* name);
void               newFrame(lua_State* ls);
void               registerLoader(lua_State* ls, lua_CFunction loader, void* userData);
const char*        getPath(lua_State* ls);
void               setPath(lua_State* ls, const char* path);
const char*        getErrorMessage(lua_State* ls, int error);
int                getMemoryInUse(lua_State* ls);
const MemoryStats& getMemoryStats(const Context* context);

class Scope {
public:
	Scope(lua_State* ls);
	~Scope();
private:
	lua_State* ls;
	LinearAllocator* tempAllocator;
	void* offs;
};

template <class T, class... ArgTypes>
inline T* newTemporary(lua_State* ls, ArgTypes... args) {
	if (void* mem = detail::allocTemporary(ls, sizeof(T), alignof(T)); mem) {
		// Construct
		return new (mem) T { std::forward<ArgTypes>(args)... };
	}
	return nullptr;
}

template <class T>
inline Reference makeRef(lua_State* ls, const T& obj) {
	push(ls, obj);
	return Reference { luaL_ref(ls, LUA_REGISTRYINDEX) };
}

} // namespace Typhoon::LuaBind

#if TY_LUABIND_NAMESPACE_ALIAS
namespace LuaBind = Typhoon::LuaBind;
#endif
