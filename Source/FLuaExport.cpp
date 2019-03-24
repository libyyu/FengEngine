
#define LUA_LIB

#include "PCH.h"
extern "C"
{
    extern void luaS_openextlibs(lua_State *L);
}
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
   // luaS_openextlibs(L);
	glb_SetLuaEnv(env);
	log_info("Setup luaState.");
}
F_DLL_API void L_SetupAnyLog(void* pfunc)
{
	glb_SetAnyLog(FLog::CreateILog(pfunc));
	
    auto lua_print = [](const char* message)
    {
        log_info(message);
    };
    auto lua_warn = [](const char* message)
    {
        log_warning(message);
    };
    auto lua_error = [](const char* message)
    {
        log_error(message);
    };
    
    LuaOutHandler::Get().SetPrintHandler(lua_print, 0);
    LuaOutHandler::Get().SetPrintHandler(lua_warn, 1);
    LuaOutHandler::Get().SetPrintHandler(lua_error, 2);
    
    log_info("AnyLog Setup!");
}

F_DLL_API void L_CleanupAnyLog()
{
	log_info("AnyLog Cleanup!");
	glb_SetAnyLog(NULL);
}

_FDeclsEnd
