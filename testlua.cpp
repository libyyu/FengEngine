#define LUA_LIB
#include "test.h"

extern "C" {
LUA_API void TestLua()
{
    int ref = 0;
    lua_State* L = luaL_newstate();
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
    LuaEnv * env = new LuaEnv();
}
}