#include "compat.h"

void socket_luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup) {
#if !defined(LUA_VERSION_NUM) || LUA_VERSION_NUM < 502
  luaL_checkstack(L, nup+1, "too many upvalues");
  for (; l->name != NULL; l++) {  /* fill the table with given functions */
    int i;
    lua_pushstring(L, l->name);
    for (i = 0; i < nup; i++)  /* copy upvalues to the top */
      lua_pushvalue(L, -(nup+1));
    lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
    lua_settable(L, -(nup + 3));
  }
  lua_pop(L, nup);  /* remove upvalues */
#else
  luaL_setfuncs(L,l,nup);
#endif
}
