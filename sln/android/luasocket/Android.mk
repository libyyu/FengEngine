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
LOCAL_C_INCLUDES  := $(LOCAL_PATH)/../../../3rd/luasocket
LOCAL_C_INCLUDES  += $(LOCAL_PATH)/../../../lua-5.1.5/src


# source files
LOCAL_SRC_FILES  := \
	$(LOCAL_PATH)/../../../3rd/luasocket/src/options.c    \
	$(LOCAL_PATH)/../../../3rd/luasocket/src/inet.c    \
	$(LOCAL_PATH)/../../../3rd/luasocket/src/io.c    \
	$(LOCAL_PATH)/../../../3rd/luasocket/src/mime.c    \
	$(LOCAL_PATH)/../../../3rd/luasocket/src/luasocket.c    \
	$(LOCAL_PATH)/../../../3rd/luasocket/src/except.c    \
	$(LOCAL_PATH)/../../../3rd/luasocket/src/usocket.c    \
	$(LOCAL_PATH)/../../../3rd/luasocket/src/tcp.c    \
	$(LOCAL_PATH)/../../../3rd/luasocket/src/buffer.c    \
	$(LOCAL_PATH)/../../../3rd/luasocket/src/select.c    \
	$(LOCAL_PATH)/../../../3rd/luasocket/src/auxiliar.c    \
	$(LOCAL_PATH)/../../../3rd/luasocket/src/serial.c    \
	$(LOCAL_PATH)/../../../3rd/luasocket/src/compat.c    \
	$(LOCAL_PATH)/../../../3rd/luasocket/src/timeout.c    \
	$(LOCAL_PATH)/../../../3rd/luasocket/src/udp.c    \
	$(LOCAL_PATH)/../../../3rd/luasocket/src/unix.c

LOCAL_SRC_FILES  +=  $(THIRD_SRCS) 


# tailer
#cmd-strip = /Users/lidengfeng/Library/Android/android-ndk-r19c/arm-linux-androideabi-4.8/prebuild/strip -s --strip-debug -x $1

ifeq ($(APP_OPTIM),release)
	LOCAL_MODULE  := luasocket
else
	LOCAL_MODULE  := luasocketDebug
endif
include $(BUILD_STATIC_LIBRARY)
