#pragma once

#include "reference.h"
#include <memory>

namespace Typhoon::LuaBind {

void unregisterObject(lua_State* ls, Reference ref);

struct RefAndState {
	lua_State* ls;
	Reference  ref;
};

struct ReferenceDeleter {
	struct pointer {
		pointer(RefAndState r)
		    : rs(r) {
		}
		pointer([[maybe_unused]] std::nullptr_t dummy = nullptr) {
			rs.ls = nullptr;
		}
		operator bool() const {
			return rs.ref.isValid();
		}
		operator RefAndState() const {
			return rs;
		}
		friend bool operator==(pointer x, pointer y) {
			return (x.rs.ls == y.rs.ls) && (x.rs.ref == y.rs.ref);
		}
		friend bool operator!=(pointer x, pointer y) {
			return ! (x == y);
		}
		RefAndState rs;
	};
	void operator()(RefAndState r) const {
		if (r.ls) {
			unregisterObject(r.ls, r.ref);
		}
	}
};

/**
 * @brief RAII wrapper for a unique Lua reference
 */
using UniqueRef = std::unique_ptr<RefAndState, ReferenceDeleter>;

inline UniqueRef makeUniqueRef(lua_State* ls, Reference ref) {
	return UniqueRef { RefAndState { ls, ref } };
}

} // namespace Typhoon::LuaBind
