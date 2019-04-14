
#define LUA_LIB

#include "PCH.h"
#include "FilePackage/FileSystem.h"
using namespace FengEngine;
_FDeclsBegin
//extern void luaS_openextlibs(lua_State *L);
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

F_DLL_API void AddFilePackageLayer(const char* path)
{
    FileSystem::Get().AddLayer(path);
    std::string sFile = "res_base/Lua/FMain.lua";
    char* pBuffer = NULL;
    long length = 0;
    bool b = FileSystem::Get().ReadFileBuffer(sFile.c_str(), &pBuffer, &length);
    log_warning("read file: %s %d", sFile.c_str(), b ? 1 : 0);
}
F_DLL_API bool ReadFileBuffer(const char* szFile, char** ppbuffer, long* length)
{
    return FileSystem::Get().ReadFileBuffer(szFile, ppbuffer, length);
}
F_DLL_API void ReleaseFileBuffer(char* pbuffer)
{
    FileSystem::Get().ReleaseFileBuffer(pbuffer);
}

_FDeclsEnd
