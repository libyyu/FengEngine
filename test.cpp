#include <iostream>
#include "flib/3rd/lua/LuaEnv.hpp"
extern "C"
{
    extern void luaS_openextlibs(lua_State *L);
}

int main()
{
    LuaEnv* env = new LuaEnv();
    luaS_openextlibs(*env);
    return 0;
}
