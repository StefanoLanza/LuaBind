#pragma once

#include "autoBlock.h"
#include "class.h"
#include "freeFunctionWrapper.h"
#include "memberFunctionWrapper.h"
#include "objectRegistration.h"
#include "stdSharedPtrWrapper.h"
#include "stdArrayWrapper.h"
#include "stdPairWrapper.h"
#include "stdVectorWrapper.h"
#include "tableTraits.h"
#include <cassert>

namespace Typhoon::LuaBind::detail {

int beginNamespace(lua_State* ls, const char* name);

}

#define LUA_BEGIN_BINDING(ls)                                      \
	__pragma(warning(push)) __pragma(warning(disable : 4127)) do { \
		using namespace LuaBind;                                   \
		lua_State* ls__ = ls;                                      \
		(void)ls__;

#define LUA_END_BINDING() \
	}                     \
	while (0)             \
	__pragma(warning(pop))

#define LUA_BEGIN_CLASS_(class, className, baseClassId)                                                                                \
	do {                                                                                                                               \
		using boundClass__ = class;                                                                                                    \
		const char* const className__ = className;                                                                                     \
		const Reference   ref = LuaBind::detail::registerCppClass(ls__, className__, Typhoon::getTypeId<boundClass__>(), baseClassId); \
		if (ref) {                                                                                                                     \
			AutoBlock autoBlock(ls__);                                                                                                 \
			lua_rawgeti(ls__, LUA_REGISTRYINDEX, ref.getValue());                                                                      \
			assert(lua_istable(ls__, -1));                                                                                             \
			const int tableStackIndex = lua_gettop(ls__);                                                                              \
			(void)tableStackIndex;

#define LUA_BEGIN_CLASS_NAMED(class, name) LUA_BEGIN_CLASS_(class, #name, Typhoon::nullTypeId)

#define LUA_BEGIN_CLASS(class) LUA_BEGIN_CLASS_(class, #class, Typhoon::nullTypeId)

#define LUA_BEGIN_SUB_CLASS(class, baseClass)           \
	static_assert(! std::is_same_v<baseClass, class>);  \
	static_assert(std::is_base_of_v<baseClass, class>); \
	LUA_BEGIN_CLASS_(class, #class, Typhoon::getTypeId<baseClass>())

#define LUA_ADD_METHOD(func)                                                                        \
	do {                                                                                            \
		LuaBind::detail::registerMemberFunction(ls__, &boundClass__::func, #func, tableStackIndex); \
	} while (0)

#define LUA_ADD_OVERLOADED_METHOD(func, ret_type, ...)                                                                                  \
	do {                                                                                                                                \
		LuaBind::detail::registerMemberFunction(ls__, static_cast<ret_type (boundClass__::*)(__VA_ARGS__)>(&boundClass__::func), #func, \
		                                        tableStackIndex);                                                                       \
	} while (0)

#define LUA_ADD_OVERLOADED_METHOD_CONST(func, ret_type, ...)                                                                                  \
	do {                                                                                                                                      \
		LuaBind::detail::registerMemberFunction(ls__, static_cast<ret_type (boundClass__::*)(__VA_ARGS__) const>(&boundClass__::func), #func, \
		                                        tableStackIndex);                                                                             \
	} while (0)

#define LUA_ADD_C_FUNCTION_RENAMED(func, funcName) \
	do {                                           \
		lua_pushliteral(ls__, funcName);           \
		lua_pushcfunction(ls__, &func);            \
		lua_settable(ls__, tableStackIndex);       \
	} while (0)

// Pick the overload with the first argument matching the bound class
template <typename StructType>
struct Overload {
	template <typename retType, typename... argType>
	static auto GetFunc(retType(func)(StructType self, argType...)) {
		return func;
	}
	template <typename retType, typename... argType>
	static auto GetFunc(retType(func)(StructType* self, argType...)) {
		return func;
	}
	template <typename retType, typename... argType>
	static auto GetFunc(retType(func)(const StructType* self, argType...)) {
		return func;
	}
	template <typename retType, typename... argType>
	static auto GetFunc(retType(func)(StructType& self, argType...)) {
		return func;
	}
	template <typename retType, typename... argType>
	static auto GetFunc(retType(func)(const StructType& self, argType...)) {
		return func;
	}
};

#define LUA_ADD_FREE_FUNCTION(function)                                                \
	do {                                                                               \
		LuaBind::detail::registerFunction(ls__, function, #function, tableStackIndex); \
	} while (0)

#define LUA_ADD_STATIC_FUNCTION(func)                                                        \
	do {                                                                                     \
		LuaBind::detail::registerFunction(ls__, boundClass__::func, #func, tableStackIndex); \
	} while (0)

#define LUA_ADD_OPERATOR(name, op)                                                                          \
	do {                                                                                                    \
		LuaBind::detail::registerMemberFunction(ls__, &boundClass__::operator##op, #name, tableStackIndex); \
	} while (0)

#define LUA_ADD_FREE_OPERATOR(name, op, ret_type, ...)                                  \
	do {                                                                                \
		LuaBind::detail::registerFunction(ls__, &operator##op, #name, tableStackIndex); \
	} while (0)

#define LUA_ADD_FREE_OPERATOR_OVERLOAD(name, op, ret_type, ...)                                                                \
	do {                                                                                                                       \
		LuaBind::detail::registerFunction(ls__, static_cast<ret_type (*)(__VA_ARGS__)>(&operator op), #name, tableStackIndex); \
	} while (0)

#define LUA_ADD_OPERATOR_OVERLOADED(name, op, ret_type, ...)                                                                                   \
	do {                                                                                                                                       \
		LuaBind::detail::registerMemberFunction(ls__, static_cast<ret_type (boundClass__::*)(__VA_ARGS__)>(&boundClass__::operator op), #name, \
		                                        tableStackIndex);                                                                              \
	} while (0)

#define LUA_ADD_OPERATOR_OVERLOADED_CONST(name, op, ret_type, ...)                                                                             \
	do {                                                                                                                                       \
		LuaBind::detail::registerMemberFunction(ls__, static_cast<ret_type (boundClass__::*)(__VA_ARGS__) const>(&boundClass__::operator##op), \
		                                        #name, tableStackIndex);                                                                       \
	} while (0)

#define LUA_GETTER(memberVar, methodName)                                                                                                 \
	do {                                                                                                                                  \
		LuaBind::detail::registerGetter(ls__, &boundClass__::memberVar, offsetof(boundClass__, memberVar), #methodName, tableStackIndex); \
	} while (0)

#define LUA_SETTER(memberVar, methodName)                                                                                                 \
	do {                                                                                                                                  \
		LuaBind::detail::registerSetter(ls__, &boundClass__::memberVar, offsetof(boundClass__, memberVar), #methodName, tableStackIndex); \
	} while (0)

#define LUA_SETTER_GETTER(memberVar, setterMethodName, getterMethodName) \
	do {                                                                 \
		LUA_SETTER(memberVar, setterMethodName);                         \
		LUA_GETTER(memberVar, getterMethodName);                         \
	} while (0)

#define LUA_END_CLASS() \
	}                   \
	}                   \
	while (0)

#define LUA_BEGIN_NAMESPACE(name)  \
	do {                           \
		AutoBlock autoBlock(ls__); \
		const int tableStackIndex = LuaBind::detail::beginNamespace(ls__, #name);

#define LUA_END_NAMESPACE() \
	}                       \
	while (0)

#define LUA_ADD_FUNCTION_RENAMED(function, functionName)                                                                   \
	do {                                                                                                                   \
		LuaBind::detail::registerFunction(ls__, Overload<boundClass__>::GetFunc(function), functionName, tableStackIndex); \
	} while (0)

#define LUA_ADD_FUNCTION(function) LUA_ADD_FUNCTION_RENAMED(function, #function)

#define LUA_ADD_OVERLOADED_FUNCTION(function, ret_type, ...)                                                                    \
	do {                                                                                                                        \
		LuaBind::detail::registerFunction(ls__, static_cast<ret_type (*)(__VA_ARGS__)>(&function), #function, tableStackIndex); \
	} while (0)

#define LUA_SET_DEFAULT_NEW_OPERATOR()                                                    \
	do {                                                                                  \
		LuaBind::detail::registerDefaultNewOperator<boundClass__>(ls__, tableStackIndex); \
	} while (0)

#define LUA_SET_NEW_OPERATOR_LUA_CFUNC(luaCFunc)                               \
	do {                                                                       \
		LuaBind::detail::registerNewOperator(ls__, tableStackIndex, luaCFunc); \
	} while (0)

#define LUA_SET_NEW_OPERATOR_TEMPORARY()                                                                                   \
	do {                                                                                                                   \
		LuaBind::detail::registerNewOperator(ls__, tableStackIndex, LuaBind::detail::createTemporaryObject<boundClass__>); \
	} while (0)

#define LUA_SET_NEW_OPERATOR(function)                                                                                                               \
	do {                                                                                                                                             \
		auto functionPtr = &function;                                                                                                                \
		LuaBind::detail::registerNewOperator(ls__, tableStackIndex, LuaBind::detail::createLuaCFunction(function), functionPtr, sizeof functionPtr); \
	} while (0)

#define LUA_BOX_OPERATOR()                                                         \
	do {                                                                           \
		LuaBind::detail::pushBoxingFunctions<boundClass__>(ls__, tableStackIndex); \
	} while (0)
