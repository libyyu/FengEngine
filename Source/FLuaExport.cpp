
#define LUA_LIB
#define FLIB_DLL
#include "PCH.h"
#include "FilePackage/FileSystem.h"
using namespace FengEngine;
_FDeclsBegin
//extern void luaS_openextlibs(lua_State *L);
FLIB_DLL_API void L_CleanupLuaState()
{
	log_info("Cleanup luaState.");
	glb_SetLuaEnv( NULL );
}
FLIB_DLL_API void L_SetupLuaState(lua_State* L)
{
	LuaEnv* env = glb_GetLuaEnv();
	SAFE_DELETE(env);
	env = new LuaEnv(L);
   // luaS_openextlibs(L);
	glb_SetLuaEnv(env);
	log_info("Setup luaState.");
}
FLIB_DLL_API void L_SetupAnyLog(void* pfunc)
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

FLIB_DLL_API void L_CleanupAnyLog()
{
	log_info("AnyLog Cleanup!");
	glb_SetAnyLog(NULL);
}

FLIB_DLL_API void L_AddFilePackageLayer(const char* path)
{
    FileSystem::Get().AddLayer(path);
}
FLIB_DLL_API bool L_ReadFileBuffer(const char* szFile, char** ppbuffer, int* length)
{
    return FileSystem::Get().ReadFileBuffer(szFile, ppbuffer, length);
}
FLIB_DLL_API void L_ReleaseFileBuffer(char* pbuffer)
{
    FileSystem::Get().ReleaseFileBuffer(pbuffer);
}
FLIB_DLL_API bool L_FileExists(const char* path)
{
    return FileSystem::Get().FileExists(path);
}

_FDeclsEnd
