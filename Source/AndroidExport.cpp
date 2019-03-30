#define LUA_LIB

#include "PCH.h"
#if PLATFORM_TARGET == PLATFORM_ANDROID
#include <jni.h>
#include <android/log.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#define LOG_TAG "AssetFILEWrapper"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  , LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , LOG_TAG, __VA_ARGS__)

static AAssetManager* l_assetManager = nullptr;
class AssetFILEWrapper
{
public:
    static void setupAssetManager(AAssetManager* assetManager)
    {
        l_assetManager = assetManager;
    }
    static bool setupAssetManager(JNIEnv* env, jobject assetManager)
    {
        AAssetManager* aAssetMgr = AAssetManager_fromJava(env, assetManager);
        if (aAssetMgr == NULL)
            return false;
        
        setupAssetManager(aAssetMgr);
        return true;
    }
    
    //open asset file in readonly mode
    static FILE* open(char const* fileName, bool read_only = true)
    {
        return openInner(l_assetManager, fileName, read_only);
    }
    
private:
    //open asset file in readonly mode
    static FILE* openInner(AAssetManager* assetManager, char const* fileName, bool read_only)
    {
        if (assetManager == NULL)
        {
            return NULL;
        }
        
        AAsset* asset = AAssetManager_open(assetManager, fileName, AASSET_MODE_RANDOM);
        if (asset == NULL)
        {
            return NULL;
        }
        
        off_t fileStart, fileLength;
        int fd = AAsset_openFileDescriptor(asset, &fileStart, &fileLength);
        AAsset_close(asset);
        
        if (fd < 0)    //fail
        {
            return NULL;
        }
        
        lseek(fd, fileStart, SEEK_SET);
        
        AssetFILEWrapper* cookie = new AssetFILEWrapper();
        cookie->m_wholeFd = fd;
        cookie->m_fileStart = fileStart;
        cookie->m_fileLen = fileLength;
        cookie->m_readonly = read_only;
        
        FILE* file = funopen(cookie, &AssetFILEWrapper::read, &AssetFILEWrapper::write, &AssetFILEWrapper::seek, &AssetFILEWrapper::close);
        if (file == NULL)
        {
            close(cookie);
            return NULL;
        }
        
        return file;
    }
    
    static int read(void *cookie, char *buffer, int buffer_size)
    {
        AssetFILEWrapper* self = (AssetFILEWrapper*)cookie;
        fpos_t curWholePos = self->tell();
        
        int leftLength = self->m_fileStart + self->m_fileLen - curWholePos;
        
        if (buffer_size > leftLength)
            buffer_size = leftLength;
        
        return ::read(self->m_wholeFd, buffer, buffer_size);
    }
    static int write(void *cookie, const char *buffer, int buffer_size)
    {
        AssetFILEWrapper* self = (AssetFILEWrapper*)cookie;
        if(self->m_readonly) return -1;
        
        return ::write(self->m_wholeFd, buffer, buffer_size);
    }
    static fpos_t seek(void *cookie, fpos_t offset, int origin)
    {
        AssetFILEWrapper* self = (AssetFILEWrapper*)cookie;
        fpos_t curWholePos = self->tell();
        fpos_t newWholePos;
        if (origin == SEEK_SET)
            newWholePos = self->m_fileStart + offset;
        else if (origin == SEEK_CUR)
            newWholePos = curWholePos + offset;
        else if (origin == SEEK_END)
            newWholePos = self->m_fileStart + self->m_fileLen + offset;
        else
            return -1;
        
        if (newWholePos < self->m_fileStart || newWholePos > self->m_fileStart + self->m_fileLen)
            return -1;
        
        ::lseek(self->m_wholeFd, newWholePos, SEEK_SET);
        return newWholePos - self->m_fileStart;
    }
    static int close(void * cookie)
    {
        AssetFILEWrapper* self = (AssetFILEWrapper*)cookie;
        ::close(self->m_wholeFd);
        
        delete self;
        return 0;
    }
    
    fpos_t tell()
    {
        return lseek(m_wholeFd, 0, SEEK_CUR);
    }
private:
    int m_wholeFd;
    off_t m_fileStart;
    off_t m_fileLen;
    bool  m_readonly;
};


static JNIEnv* g_JniEnv = NULL;
JNIEnv* glb_GetJniEnv()
{
    return g_JniEnv;
}

void setupAssetFILEWrapper()
{
    JNIEnv* env = glb_GetJniEnv();
    jclass UnityPlayer = env->FindClass("com/unity3d/player/UnityPlayer");
    if (UnityPlayer == NULL)
    {
        LOGI("com/unity3d/player/UnityPlayer not found");
        return;
    }
    
    jfieldID currentActivityId = env->GetStaticFieldID(UnityPlayer, "currentActivity", "Landroid/app/Activity;");
    if (currentActivityId == NULL)
    {
        LOGI("currentActivity not found");
        return;
    }
    
    jobject activity = env->GetStaticObjectField(UnityPlayer, currentActivityId);
    if (activity == NULL)
    {
        LOGI("activity not found");
        return;
    }
    
    jclass Activity = env->FindClass("android/app/Activity");
    if (Activity == NULL)
    {
        LOGI("android/app/Activity not found");
        return;
    }
    
    jmethodID getAssetsId = env->GetMethodID(Activity, "getAssets", "()Landroid/content/res/AssetManager;");
    if (getAssetsId == NULL)
    {
        LOGI("getAssets not found");
        return;
    }
    
    jobject assets = env->CallObjectMethod(activity, getAssetsId);
    if (assets == NULL)
    {
        LOGI("assets not found");
        return;
    }
    //
    AssetFILEWrapper::setupAssetManager(env, assets);
}
///////////////////////////////////////////////////////////////////////////
////
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    LOGD("hello in c native code.\n");
    jint result = -1;

    if (vm->GetEnv((void**) &g_JniEnv, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
    assert(g_JniEnv != NULL);

    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

    return result;
}

#endif
