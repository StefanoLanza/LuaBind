#pragma once

#include "typeWrapper.h"
#include <memory>

namespace Typhoon::LuaBind {

// Wrapper for std::shared_ptr
// FIXME This is going to crash, rewrite in order to keep the ref count
template <class T>
class Wrapper<std::shared_ptr<T>> {
public:
	static int Match(lua_State* ls, int idx) {
		const int type = lua_type(ls, idx);
		return (type == LUA_TLIGHTUSERDATA || type == LUA_TUSERDATA || type == LUA_TTABLE || type == LUA_TNIL);
	}
	static int Push(lua_State* ls, const std::shared_ptr<T>& ptr) {
		return Wrapper<T*>.Push(ls, ptr.get());
	};
#if 0
	static int Get(lua_State* /*ls*/, int /*idx*/, std::shared_ptr<Type>& /*sharedPtr*/) 
	{
		void* rawPtr = nullptr;
		if (lua_islightuserdata(ls, idx))
		{
			rawPtr = lua_touserdata(ls, idx);
		}
		else 
		{
			// Table
			if (! detail::GetRawPointer(ls, idx, rawPtr) ) 
			{
				return 0;
			}
		}
		sharedPtr = std::make_shared(static_cast<Type*>(rawPtr));
		return 1;
	}
#endif
	static std::shared_ptr<T> Get(lua_State* /*ls*/, int /*idx*/) {
		return {};
	}
	static constexpr int stackSize = 1;
};

} // namespace Typhoon::LuaBind
