#pragma once

#include "typeWrapper.h"
#include <memory>

namespace Typhoon::LuaBind {

// Wrapper for std::shared_ptr
// FIXME This is going to crash, rewrite in order to keep the ref count
template <class T>
class Wrapper<std::shared_ptr<T>> {
public:
	static constexpr int stackSize = 1;

	static int match(lua_State* ls, int idx) {
		return Wrapper<T*>::match(ls, idx);
	}
	static void push(lua_State* ls, const std::shared_ptr<T>& ptr) {
		Wrapper<T*>::push(ls, ptr.get());
	}
#if 0
	static int pop(lua_State* /*ls*/, int /*idx*/, std::shared_ptr<Type>& /*sharedPtr*/) 
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
	static std::shared_ptr<T> pop(lua_State* /*ls*/, int /*idx*/) {
		return {};
	}
#endif
};

} // namespace Typhoon::LuaBind
