#pragma once

// Not yet fully supported
#ifndef TY_LUABIND_TYPE_SAFE
#define TY_LUABIND_TYPE_SAFE 0
#endif

// Set to 1 to create the LuaBind alias for the namespace Typhoon::LuaBind
#ifndef TY_LUABIND_NAMESPACE_ALIAS
#define TY_LUABIND_NAMESPACE_ALIAS 1
#endif

// Set to 1 to create new objects with a new method e.g.
// Class.new(args)
// Set to 0 to create new objects via the __call metamethod e.g.
// Class(args)
#ifndef TY_LUABIND_NEW_OR_CALL
#define TY_LUABIND_NEW_OR_CALL 1
#endif
