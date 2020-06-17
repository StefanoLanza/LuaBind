#include "autoBlock.h"
#include "table.h"
#include <cassert>
#include <type_traits>

namespace Typhoon::LUA {

template <class T>
T* newTemporary() {
	void* mem = detail::allocTemporary(sizeof(T), std::alignment_of_v<T>);
	if (mem) {
		// Construct
		return new (mem) T;
	}
	return nullptr;
}

} // namespace Typhoon::LUA

namespace Typhoon::LUA::detail {

template <class Type>
int GarbageCollect(lua_State* ls) {
	// Extract pointer from user data
	Type** const ptrptr = static_cast<Type**>(lua_touserdata(ls, 1));
	assert(ptrptr);
	// Delete object
	Type* const obj = *ptrptr;
	assert(obj);
	delete obj;
	return 0;
}

template <typename T>
int createTemporaryObject(lua_State* ls) {
	// Allocate user data in temporary memory and construct new object in that memory
	T* const ud = newTemporary<T>();
	lua_pushlightuserdata(ls, ud);
	return 1;
}

template <typename Type>
void pushDestructor(lua_State* ls) {
	// Set metatable for userdata (required for __gc)
	lua_newtable(ls);                            // mt, ud
	lua_pushvalue(ls, -1);                       // mt, mt, ud
	lua_pushcfunction(ls, GarbageCollect<Type>); // func, mt, mt, ud
	lua_setfield(ls, -2, "__gc");                // mt, mt, ud
	lua_setmetatable(ls, -3);                    // mt, ud
	lua_pop(ls, 1);                              // ud
}

template <typename Type>
int newObject(lua_State* ls) {
	Type* const ptr = new Type;
	// Allocate full user data
	void* const ud = lua_newuserdata(ls, sizeof(ptr));
	// Store the object pointer in the user data
	std::memcpy(ud, &ptr, sizeof(ptr));
	// Push destructor if not trivially destructible
	if constexpr (! std::is_trivially_destructible_v<Type>) {
		pushDestructor<Type>(ls);
	}
#if LUA_TYPE_SAFE
	// TODO in the userdata instead?
	detail::registerPointerType(ptr, getTypeId<Type>());
#endif
	return 1;
}

template <class T>
int Box(lua_State* ls) {
	void* mem = allocateBoxed(sizeof(T), std::alignment_of_v<T>);
	T* boxed = new (mem) T;

	// Allocate full user data
	void* const ud = lua_newuserdata(ls, sizeof boxed);
	// Construct new object and store a pointer to its pointer in the user data
	std::memcpy(ud, &boxed, sizeof boxed);

	// Set metatable for userdata (required for __gc)
	lua_newtable(ls);                    // mt, ud
	lua_pushvalue(ls, -1);               // mt, mt, ud
	lua_pushcfunction(ls, CollectBoxed); // func, mt, mt, ud
	lua_setfield(ls, -2, "__gc");        // mt, mt, ud
	lua_setmetatable(ls, -3);            // mt, ud

	lua_pop(ls, 1); // ud

	return 1;
}

template <class Type>
int Store(lua_State* ls) {
	// Stack: boxed (ud), value(ud)
	assert(lua_isuserdata(ls, 1));
	assert(lua_isuserdata(ls, 2));

	Type* boxed = nullptr;
	std::memcpy(&boxed, lua_touserdata(ls, 1), sizeof boxed);
	// Store value in boxed object
	*boxed = Wrapper<Type>::Get(ls, 2);
	return 0;
}

template <class Type>
int Retrieve(lua_State* ls) {
	// Stack: boxed (ud)
	if (! lua_isuserdata(ls, 1)) {
		return 0;
	}
	const Type* boxed = nullptr;
	std::memcpy(&boxed, lua_touserdata(ls, 1), sizeof boxed);
	// Retrieve value stored in boxed object
	return Wrapper<Type>::Push(ls, *boxed);
}

template <class Class>
void PushBoxingFunctions(lua_State* ls, int tableStackIndex) {
	lua_pushliteral(ls, "Box");
	lua_pushcfunction(ls, Box<Class>);
	lua_settable(ls, tableStackIndex);
	lua_pushliteral(ls, "Store");
	lua_pushcfunction(ls, Store<Class>);
	lua_settable(ls, tableStackIndex);
	lua_pushliteral(ls, "Retrieve");
	lua_pushcfunction(ls, Retrieve<Class>);
	lua_settable(ls, tableStackIndex);
}

template <class T>
void PushObjectAsFullUserData(lua_State* ls, T* objectPtr) {
	assert(objectPtr);
	const char* const classID = GetClassID(GetTypeInfo(objectPtr));
	PushObjectAsFullUserData(ls, static_cast<void*>(objectPtr), classID);
}
} // namespace Typhoon::LUA::detail
