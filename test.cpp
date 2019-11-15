#include <iostream>
#include "flib/3rd/lua/LuaEnv.hpp"
#include "File.h"

_FDeclsBegin
_FCFun void luaS_openextlibs(lua_State *L);
_FDeclsEnd
using namespace std;
string convert(const string& s, int numRows) {
    if(numRows==1 || s.length()<=1) return s;
    int cnt = s.length();
    int oneMidume = numRows-2; //
    int oneZnum = 2*numRows + oneMidume;//num of one 'Z'
    int zNum = cnt/(numRows+oneMidume);
    int zColNum = zNum*(numRows-1);
    if(cnt%(numRows+oneMidume)) {
        int left = cnt-zNum*(numRows-1);
        int r = left/numRows + left%numRows;
        zColNum += r;
        zNum = zNum + 1;
    }
    vector<vector<char> > arrs;
    for(int r=0; r<numRows;++r){
        std::vector<char> t;
        for(int c=0; c<zColNum; ++c){
            t.push_back(' ');
        }
        arrs.push_back(t);
    }
    int r = -1;
    int c = 0;
    int rsign = 1;
    int csign = 0;
    for(int i=0; i<cnt;++i){
        if(r==-1 or r==0){
            rsign = 1;
            csign = 0;
        }
        else if(r==numRows-1){
            rsign = -1;
            csign = 1;
        }
        r += rsign;
        c += csign;
        arrs[r][c] = s[i];
    }
    string o;
    for(int r=0; r<numRows;++r){
        for(int c=0; c<zColNum; ++c){
            if (arrs[r][c] != ' ')
                o += arrs[r][c];
        }
    }
    return o;
}

int main()
{
    LuaEnv* env = new LuaEnv();
    luaS_openextlibs(*env);
    
    FStd::File* file = FStd::File::OpenFile("/Users/lidengfeng/Documents/Workspace/FengEngine/a.txt");
    size_t len = file->GetSize();
    char* buffer = new char[len+1];
    file->ReadAll(buffer);
    buffer[len] = 0x0;
    env->doGlobal("print", buffer);
    delete []buffer;
    delete file;
    delete env;
    
    auto s = convert("LEETCODEISHIRING",3);
    
    return 0;
}
