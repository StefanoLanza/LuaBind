namespace Typhoon::LuaBind {

template <class KeyType, class ValueType>
void Table::set(const KeyType& key, const ValueType& value) {
	static_assert(Wrapper<const KeyType&>::stackSize == 1);
	static_assert(Wrapper<const ValueType&>::stackSize == 1);

	assert(isValid());
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	Wrapper<const KeyType&>::pushAsKey(ls, key);
	Wrapper<const ValueType&>::push(ls, value);
	lua_settable(ls, -3);
	lua_pop(ls, 1); // table
}

template <class KeyType, class ValueType>
void Table::rawSet(const KeyType& key, const ValueType& value) {
	static_assert(Wrapper<const KeyType&>::stackSize == 1);
	static_assert(Wrapper<const ValueType&>::stackSize == 1);

	assert(isValid());
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	Wrapper<const KeyType&>::pushAsKey(ls, key);
	Wrapper<const ValueType&>::push(ls, value);
	lua_rawset(ls, -3);
	lua_pop(ls, 1); // table
}

template <class ValueType>
void Table::rawSeti(lua_Integer i, const ValueType& value) {
	static_assert(Wrapper<const ValueType&>::stackSize == 1);

	assert(isValid());
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	Wrapper<const ValueType&>::push(ls, value);
	// table[i] = value
	lua_rawseti(ls, -2, i);
	lua_pop(ls, 1); // table
}

template <class KeyType>
Value Table::operator[](const KeyType& key) const {
	static_assert(Wrapper<const KeyType&>::stackSize == 1);

	assert(isValid());
	lua_rawgeti(ls, LUA_REGISTRYINDEX, ref);
	Wrapper<const KeyType&>::pushAsKey(ls, key);
	lua_gettable(ls, -2);
	Value value { ls, topStackIndex };
	lua_pop(ls, 2); // table, value
	return value;
}

} // namespace Typhoon::LuaBind
