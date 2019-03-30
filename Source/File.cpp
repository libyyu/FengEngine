#include "PCH.h"
#include "File.h"
#include "flib/base/FFile.hpp"

File::File() : m_pFileHandle(nullptr), m_IsAndroidAssets(false)
{
}
File::~File()
{
    close();
}

void File::close()
{
    if(m_pFileHandle)
    {
        if(!m_IsAndroidAssets)
        {
            FStd::FFile* fp = static_cast<FStd::FFile*>(m_pFileHandle);
            delete fp;
        }
        else
        {
            FILE* fp = static_cast<FILE*>(m_pFileHandle);
            fclose(fp);
        }
    }
    m_pFileHandle = (nullptr);
}

size_t File::GetSize()
{
    if(!m_IsAndroidAssets)
    {
        FStd::FFile* fp = static_cast<FStd::FFile*>(m_pFileHandle);
        return fp->GetSize();
    }
    else
    {
        FILE* fp = static_cast<FILE*>(m_pFileHandle);
        size_t offset = GetOffset();
        size_t filelen = fseek(fp, 0L, SEEK_END);
        fseek(fp, offset, SEEK_SET);
        return filelen-offset;
    }
}
size_t File::GetOffset()
{
    if(!m_IsAndroidAssets)
    {
        FStd::FFile* fp = static_cast<FStd::FFile*>(m_pFileHandle);
        return fp->GetOffset();
    }
    else
    {
        FILE* fp = static_cast<FILE*>(m_pFileHandle);
        return ftell(fp);
    }
}
int File::Seek(int offset, unsigned int mode)
{
    if(!m_IsAndroidAssets)
    {
        FStd::FFile* fp = static_cast<FStd::FFile*>(m_pFileHandle);
        return fp->Seek(offset, mode);
    }
    else
    {
        FILE* fp = static_cast<FILE*>(m_pFileHandle);
        return fseek(fp, offset, mode);
    }
}
size_t File::Read(void* p_buffer, size_t n_bytes_2_read)
{
    assert(p_buffer);
    if(!m_IsAndroidAssets)
    {
        FStd::FFile* fp = static_cast<FStd::FFile*>(m_pFileHandle);
        return fp->Read(p_buffer, n_bytes_2_read);
    }
    else
    {
        FILE* fp = static_cast<FILE*>(m_pFileHandle);
        return fread(p_buffer, n_bytes_2_read, 1, fp);
    }
}
size_t File::Write(const void* p_buffer, size_t n_bytes_2_write)
{
    assert(p_buffer);
    if(!m_IsAndroidAssets)
    {
        FStd::FFile* fp = static_cast<FStd::FFile*>(m_pFileHandle);
        return fp->Write(p_buffer, n_bytes_2_write);
    }
    else
    {
        FILE* fp = static_cast<FILE*>(m_pFileHandle);
        return fwrite(p_buffer, n_bytes_2_write, 1, fp);
    }
}
size_t File::ReadAll(void* p_buffer)
{
    assert(p_buffer);
    if(!m_IsAndroidAssets)
    {
        FStd::FFile* fp = static_cast<FStd::FFile*>(m_pFileHandle);
        return fp->ReadAll(p_buffer);
    }
    else
    {
        FILE* fp = static_cast<FILE*>(m_pFileHandle);
        size_t len = GetSize();
        return fread(p_buffer, len, 1, fp);
    }
}

#if PLATFORM_TARGET != PLATFORM_ANDROID
File* File::OpenFile(const char *szFileName, bool readonly)
{
    assert(szFileName && "szFileName is null");
    FStd::FFile* fp = new FStd::FFile;
    if(0 != fp->Open(szFileName, readonly))
    {
        delete fp;
        return nullptr;
    }
    
    File* file = new File;
    file->m_pFileHandle = fp;
    file->m_IsAndroidAssets = false;
    return file;
}
#endif
