#pragma once

#include "reference.h"
#include <memory>

namespace Typhoon::LuaBind {

void unregisterObject(lua_State* ls, Reference ref);

struct RefDeleter {
	struct pointer {
		Reference r;
		pointer(std::nullptr_t = nullptr) :
			r {}
		{
		}
		pointer(Reference r)
		    : r(r) {
		}
		operator bool() const {
			return r.isValid();
		}
		operator Reference() const {
			return r;
		}
		Reference ref() const {
			return r;
		}
		friend bool operator==(pointer x, pointer y) {
			return x.r == y.r;
		}
		friend bool operator!=(pointer x, pointer y) {
			return ! (x == y);
		}
	};
	void operator()(Reference r) const {
		//unregisterObject(r.ls, r.ref);
	}
};

/**
 * @brief RAII wrapper for a unique Lua reference
*/
using UniqueRef = std::unique_ptr<Reference, RefDeleter>;


} // namespace Typhoon::LuaBind
