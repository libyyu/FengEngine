
#define _F_DLL_
#define LUA_LIB

#include "PCH.h"

_FDeclsBegin

F_DLL_API void L_CleanupLuaState()
{
	log_info("Cleanup luaState.");
	glb_SetLuaEnv( NULL );
}
F_DLL_API void L_SetupLuaState(lua_State* L)
{
	LuaEnv* env = glb_GetLuaEnv();
	SAFE_DELETE(env);
	env = new LuaEnv(L);
	glb_SetLuaEnv(env);
	log_info("Setup luaState.");
}
F_DLL_API void L_SetupAnyLog(void* pfunc)
{
	glb_SetAnyLog(FLog::CreateILog(pfunc));
	log_info("AnyLog Setup!");
}

F_DLL_API void L_CleanupAnyLog()
{
	log_info("AnyLog Cleanup!");
	glb_SetAnyLog(NULL);
}

_FDeclsEnd
