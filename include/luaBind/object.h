#pragma once

#include "result.h"
#include "reference.h"
#include <utility>
#include <cassert>

struct lua_State;

namespace Typhoon::LuaBind {

/**
 * @brief 
*/
class Object {
public:
	/**
	 * @brief Constructor
	 * @param ls lua state
	 * @param ref object reference
	*/
	Object(lua_State* ls, Reference ref);

	/**
	 * @brief 
	 * @param func 
	 * @return 
	*/
	bool hasMethod(const char* func) const;

	/**
	 * @brief 
	 * @param func 
	 * @return 
	*/
	Result callMethod(const char* func) const;

	/**
	 * @brief 
	 * @tparam RetType 
	 * @tparam ...ArgTypes 
	 * @param func 
	 * @param ret 
	 * @param ...args 
	 * @return 
	*/
	template <typename RetType, typename... ArgTypes>
	Result callMethodRet(const char* func, RetType& ret, ArgTypes&&... args) const;

	/**
	 * @brief 
	 * @tparam ...ArgTypes 
	 * @param func 
	 * @param ...args 
	 * @return 
	*/
	template <typename... ArgTypes>
	Result callMethod(const char* func, ArgTypes&&... args) const;

private:
	std::pair<bool, int> beginCall(const char* func) const;
	Result               callMethodImpl(int narg, int nres) const;

private:
	lua_State* ls;
	int        ref;
};

} // namespace Typhoon::LuaBind


#include "object.inl"
