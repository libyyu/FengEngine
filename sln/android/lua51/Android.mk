LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# preinclude static libs
ifeq ($(APP_OPTIM)_$(TARGET_ARCH_ABI),release_armeabi-v7a)
else ifeq ($(APP_OPTIM)_$(TARGET_ARCH_ABI),release_x86)
else ifeq ($(APP_OPTIM)_$(TARGET_ARCH_ABI),debug_armeabi-v7a)
else ifeq ($(APP_OPTIM)_$(TARGET_ARCH_ABI),debug_x86)
endif

include $(CLEAR_VARS)

# flags
LOCAL_CPP_FEATURES := rtti exceptions


# defines
LOCAL_CFLAGS += -D _GNU_SOURCE=1
LOCAL_CFLAGS += -D _FILE_OFFSET_BITS=64
LOCAL_CFLAGS += -D _LARGEFILE_SOURCE
LOCAL_CFLAGS += -D __ANDROID__
LOCAL_CFLAGS += -std=gnu99
LOCAL_CFLAGS += -Wno-error=deprecated-declarations -fno-strict-aliasing -fomit-frame-pointer
LOCAL_CPPFLAGS += -Wno-error=deprecated-declarations -fno-strict-aliasing -fomit-frame-pointer


# includedirs
LOCAL_C_INCLUDES  := $(LOCAL_PATH)/../../../lua-5.1.5


# source files
LOCAL_SRC_FILES  := \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/lopcodes.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/loslib.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/ldump.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/ltm.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/linit.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/lobject.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/ltablib.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/lparser.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/lmathlib.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/ldebug.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/lmem.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/lundump.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/lstate.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/lbaselib.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/lauxlib.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/ltable.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/lvm.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/lfunc.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/lapi.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/llex.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/ldo.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/lstring.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/lzio.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/ldblib.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/lgc.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/lcode.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/lbit.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/loadlib.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/liolib.c    \
	$(LOCAL_PATH)/../../../lua-5.1.5/src/lstrlib.c

LOCAL_SRC_FILES  +=  $(THIRD_SRCS) 


# tailer
#cmd-strip = /Users/lidengfeng/Library/Android/android-ndk-r19c/arm-linux-androideabi-4.8/prebuild/strip -s --strip-debug -x $1

ifeq ($(APP_OPTIM),release)
	LOCAL_MODULE  := lua51
else
	LOCAL_MODULE  := lua51Debug
endif
include $(BUILD_STATIC_LIBRARY)
