#include <iostream>
#include "flib/3rd/lua/LuaEnv.hpp"
#include "File.h"

extern "C"
{
    extern void luaS_openextlibs(lua_State *L);
}

int main()
{
    LuaEnv* env = new LuaEnv();
    luaS_openextlibs(*env);
    
    File* file = File::OpenFile("/Users/lidengfeng/Documents/Workspace/FengEngine/a.txt");
    size_t len = file->GetSize();
    char* buffer = new char[len+1];
    file->ReadAll(buffer);
    buffer[len] = 0x0;
    
    delete []buffer;
    delete file;
    delete env;
    return 0;
}
