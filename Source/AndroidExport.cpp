#define LUA_LIB

#include "PCH.h"

#if PLATFORM_TARGET == PLATFORM_ANDROID
#include <jni.h>
#include <android/log.h>

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, MODULE_NAME, __VA_ARGS__) 
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG , MODULE_NAME, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO  , MODULE_NAME, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN  , MODULE_NAME, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , MODULE_NAME, __VA_ARGS__)

static JNIEnv* g_JniEnv = NULL;
JNIEnv* glb_GetJniEnv()
{
    return g_JniEnv;
}

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
