LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# preinclude static libs
ifeq ($(APP_OPTIM)_$(TARGET_ARCH_ABI),release_armeabi-v7a)
	include $(CLEAR_VARS)
	LOCAL_MODULE        := lua51
	LOCAL_SRC_FILES     := $(LOCAL_PATH)/../lua51/obj/local/armeabi-v7a/liblua51.a
	THIRD_MODULS        := $(LOCAL_MODULE)
	include $(PREBUILT_STATIC_LIBRARY)
	include $(CLEAR_VARS)
	LOCAL_MODULE        := thirdpart
	LOCAL_SRC_FILES     := $(LOCAL_PATH)/../thirdpart/obj/local/armeabi-v7a/libthirdpart.a
	THIRD_MODULS        += $(LOCAL_MODULE)
	include $(PREBUILT_STATIC_LIBRARY)
	include $(CLEAR_VARS)
	LOCAL_MODULE        := cjson
	LOCAL_SRC_FILES     := $(LOCAL_PATH)/../cjson/obj/local/armeabi-v7a/libcjson.a
	THIRD_MODULS        += $(LOCAL_MODULE)
	include $(PREBUILT_STATIC_LIBRARY)
	include $(CLEAR_VARS)
	LOCAL_MODULE        := luasocket
	LOCAL_SRC_FILES     := $(LOCAL_PATH)/../luasocket/obj/local/armeabi-v7a/libluasocket.a
	THIRD_MODULS        += $(LOCAL_MODULE)
	include $(PREBUILT_STATIC_LIBRARY)
else ifeq ($(APP_OPTIM)_$(TARGET_ARCH_ABI),release_x86)
	include $(CLEAR_VARS)
	LOCAL_MODULE        := lua51
	LOCAL_SRC_FILES     := $(LOCAL_PATH)/../lua51/obj/local/x86/liblua51.a
	THIRD_MODULS        := $(LOCAL_MODULE)
	include $(PREBUILT_STATIC_LIBRARY)
	include $(CLEAR_VARS)
	LOCAL_MODULE        := thirdpart
	LOCAL_SRC_FILES     := $(LOCAL_PATH)/../thirdpart/obj/local/x86/libthirdpart.a
	THIRD_MODULS        += $(LOCAL_MODULE)
	include $(PREBUILT_STATIC_LIBRARY)
	include $(CLEAR_VARS)
	LOCAL_MODULE        := cjson
	LOCAL_SRC_FILES     := $(LOCAL_PATH)/../cjson/obj/local/x86/libcjson.a
	THIRD_MODULS        += $(LOCAL_MODULE)
	include $(PREBUILT_STATIC_LIBRARY)
	include $(CLEAR_VARS)
	LOCAL_MODULE        := luasocket
	LOCAL_SRC_FILES     := $(LOCAL_PATH)/../luasocket/obj/local/x86/libluasocket.a
	THIRD_MODULS        += $(LOCAL_MODULE)
	include $(PREBUILT_STATIC_LIBRARY)
else ifeq ($(APP_OPTIM)_$(TARGET_ARCH_ABI),debug_armeabi-v7a)
	include $(CLEAR_VARS)
	LOCAL_MODULE        := lua51Debug
	LOCAL_SRC_FILES     := $(LOCAL_PATH)/../lua51/obj/local/armeabi-v7a/liblua51Debug.a
	THIRD_MODULS        := $(LOCAL_MODULE)
	include $(PREBUILT_STATIC_LIBRARY)
	include $(CLEAR_VARS)
	LOCAL_MODULE        := thirdpartDebug
	LOCAL_SRC_FILES     := $(LOCAL_PATH)/../thirdpart/obj/local/armeabi-v7a/libthirdpartDebug.a
	THIRD_MODULS        += $(LOCAL_MODULE)
	include $(PREBUILT_STATIC_LIBRARY)
	include $(CLEAR_VARS)
	LOCAL_MODULE        := cjsonDebug
	LOCAL_SRC_FILES     := $(LOCAL_PATH)/../cjson/obj/local/armeabi-v7a/libcjsonDebug.a
	THIRD_MODULS        += $(LOCAL_MODULE)
	include $(PREBUILT_STATIC_LIBRARY)
	include $(CLEAR_VARS)
	LOCAL_MODULE        := luasocketDebug
	LOCAL_SRC_FILES     := $(LOCAL_PATH)/../luasocket/obj/local/armeabi-v7a/libluasocketDebug.a
	THIRD_MODULS        += $(LOCAL_MODULE)
	include $(PREBUILT_STATIC_LIBRARY)
else ifeq ($(APP_OPTIM)_$(TARGET_ARCH_ABI),debug_x86)
	include $(CLEAR_VARS)
	LOCAL_MODULE        := lua51Debug
	LOCAL_SRC_FILES     := $(LOCAL_PATH)/../lua51/obj/local/x86/liblua51Debug.a
	THIRD_MODULS        := $(LOCAL_MODULE)
	include $(PREBUILT_STATIC_LIBRARY)
	include $(CLEAR_VARS)
	LOCAL_MODULE        := thirdpartDebug
	LOCAL_SRC_FILES     := $(LOCAL_PATH)/../thirdpart/obj/local/x86/libthirdpartDebug.a
	THIRD_MODULS        += $(LOCAL_MODULE)
	include $(PREBUILT_STATIC_LIBRARY)
	include $(CLEAR_VARS)
	LOCAL_MODULE        := cjsonDebug
	LOCAL_SRC_FILES     := $(LOCAL_PATH)/../cjson/obj/local/x86/libcjsonDebug.a
	THIRD_MODULS        += $(LOCAL_MODULE)
	include $(PREBUILT_STATIC_LIBRARY)
	include $(CLEAR_VARS)
	LOCAL_MODULE        := luasocketDebug
	LOCAL_SRC_FILES     := $(LOCAL_PATH)/../luasocket/obj/local/x86/libluasocketDebug.a
	THIRD_MODULS        += $(LOCAL_MODULE)
	include $(PREBUILT_STATIC_LIBRARY)
endif

include $(CLEAR_VARS)

# flags
LOCAL_CPP_FEATURES := rtti exceptions


# defines
LOCAL_CFLAGS += -D _GNU_SOURCE=1
LOCAL_CFLAGS += -D _FILE_OFFSET_BITS=64
LOCAL_CFLAGS += -D _LARGEFILE_SOURCE
LOCAL_CFLAGS += -D __ANDROID__
LOCAL_CFLAGS += -D _F_DLL_
LOCAL_CFLAGS += -std=c99
LOCAL_CPPFLAGS += -std=c++11
LOCAL_CFLAGS += -Wno-error=deprecated-declarations -fno-strict-aliasing -fomit-frame-pointer
LOCAL_CPPFLAGS += -Wno-error=deprecated-declarations -fno-strict-aliasing -fomit-frame-pointer


# includedirs
LOCAL_C_INCLUDES  := $(LOCAL_PATH)/../../..
LOCAL_C_INCLUDES  += $(LOCAL_PATH)/../../../flib
LOCAL_C_INCLUDES  += $(LOCAL_PATH)/../../../lua-5.1.5/src


# source files
LOCAL_SRC_FILES  := \
	$(LOCAL_PATH)/../../../Source/AndroidExport.cpp    \
	$(LOCAL_PATH)/../../../Source/AnyLog/ILog.cpp    \
	$(LOCAL_PATH)/../../../Source/PCH.cpp    \
	$(LOCAL_PATH)/../../../Source/WindowsExport.cpp    \
	$(LOCAL_PATH)/../../../Source/FLuaExport.cpp    \
	$(LOCAL_PATH)/../../../Source/File.cpp

LOCAL_SRC_FILES  +=  $(THIRD_SRCS) 


# tailer
LOCAL_SHARED_LIBARIES :=\
libcutils \
libdl

LOCAL_EXPORT_LDFLAGS += --whole-archive
#LOCAL_STATIC_LIBRARIES := $(THIRD_MODULS)
LOCAL_WHOLE_STATIC_LIBRARIES := $(THIRD_MODULS)
LOCAL_LDLIBS += -landroid -llog -ljnigraphics

#cmd-strip = /Users/lidengfeng/Library/Android/android-ndk-r19c/arm-linux-androideabi-4.8/prebuild/strip -s --strip-debug -x $1

ifeq ($(APP_OPTIM),release)
	LOCAL_MODULE  := FengEngine
else
	LOCAL_MODULE  := FengEngineDebug
endif
include $(BUILD_SHARED_LIBRARY)
