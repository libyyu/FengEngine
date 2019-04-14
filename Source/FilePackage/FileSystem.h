#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__
#pragma once
#include <vector>
#include <string>
namespace FengEngine
{
class FileSystem
{
public:
    FileSystem();
    ~FileSystem();
    static FileSystem& Get();
public:
    void AddLayer(const char* path);
    bool ReadFileBuffer(const char* szFile, char** ppbuffer, long* length);
    void ReleaseFileBuffer(char* ppbuffer);
private:
    std::vector<std::string> layers;
};
}

#endif//__FILE_SYSTEM_H__
