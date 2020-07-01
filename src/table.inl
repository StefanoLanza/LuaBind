namespace Typhoon::LuaBind {

template <class KeyType, class ValueType>
void Table::set(const KeyType& key, const ValueType& value) {
	assert(isValid());
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	pushAsKey(ls, key);
	push(ls, value);
	lua_settable(ls, -3);
	lua_pop(ls, 1); // table
}

template <class KeyType, class ValueType>
void Table::rawSet(const KeyType& key, const ValueType& value) {
	assert(isValid());
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	pushAsKey(ls, key);
	push(ls, value);
	lua_rawset(ls, -3);
	lua_pop(ls, 1); // table
}

template <class ValueType>
void Table::rawSeti(int i, const ValueType& value) {
	assert(isValid());
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	push(ls, value);
	// table[i] = value
	lua_rawseti(ls, -2, i);
	lua_pop(ls, 1); // table
}

} // namespace Typhoon::LuaBind
