
#define _F_DLL_
#define LUA_LIB

#include "Common/FAssist.h"

_RzDeclsBegin

RZ_DLL_API void L_CleanupLuaState()
{
	log_info("Cleanup luaState.");
	g_SetLuaState( NULL );
}
RZ_DLL_API void L_SetupLuaState(lua_State* l)
{
	g_SetLuaState(l);
	log_info("Setup luaState.");
}
RZ_DLL_API void L_EstablishAnyLog(void* pfunc)
{
	g_SetAnyLog(FLog::CreateILog(pfunc));
	log_info("AnyLog Established!");
}

RZ_DLL_API void L_UnEstablishAnyLog()
{
	log_info("AnyLog UnEstablish!");
	g_SetAnyLog(NULL);
}


_RzDeclsEnd
