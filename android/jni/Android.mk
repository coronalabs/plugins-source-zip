# Copyright (C) 2012 Corona Labs Inc.
#

# TARGET_PLATFORM := android-14

LOCAL_PATH:= $(call my-dir)
CORONA_ENTERPRISE:=/Applications/CoronaEnterprise
CORONA_ROOT:=$(CORONA_ENTERPRISE)/Corona


# -----------------------------------------------------------------------------

include $(CLEAR_VARS)
LOCAL_MODULE := corona
LOCAL_SRC_FILES := ../libcorona.so
#LOCAL_EXPORT_C_INCLUDES := $(LUA_API_DIR)
include $(PREBUILT_SHARED_LIBRARY)

# -----------------------------------------------------------------------------


include $(CLEAR_VARS)
LOCAL_MODULE := lua
LOCAL_SRC_FILES := ../liblua.so
#LOCAL_EXPORT_C_INCLUDES := $(LUA_API_DIR)
include $(PREBUILT_SHARED_LIBRARY)

# -----------------------------------------------------------------------------


include $(CLEAR_VARS)

LOCAL_MODULE := libplugin.zip

PLUGIN_DIR      	:= ../..
PLUGIN_ADDITIONAL	:= $(PLUGIN_DIR)/shared/minizip
CORONA_API_DIR  	:= $(CORONA_ROOT)/shared/include/Corona
LUA_API_DIR     	:= $(CORONA_ROOT)/shared/include/lua

LOCAL_C_INCLUDES := \
	$(PLUGIN_DIR) \
	$(CORONA_API_DIR) \
	$(LUA_API_DIR) \
	$(PLUGIN_ADDITIONAL)

LOCAL_SRC_FILES  := \
	$(PLUGIN_DIR)/shared/minizip/ioapi.c \
	$(PLUGIN_DIR)/shared/minizip/unzip.c \
	$(PLUGIN_DIR)/shared/minizip/zip.c \
	$(PLUGIN_DIR)/shared/AsyncTask.cpp \
	$(PLUGIN_DIR)/shared/ZipEvent.cpp \
	$(PLUGIN_DIR)/shared/ZipLibrary.cpp \
	$(PLUGIN_DIR)/shared/ZipTask.cpp \
	$(PLUGIN_DIR)/shared/AsyncZip.cpp \


LOCAL_CFLAGS     := \
	-DANDROID_NDK \
	-DNDEBUG \
	-D_REENTRANT \
	-DRtt_ANDROID_ENV \
	-D__ANDROID__

LOCAL_SHARED_LIBRARIES := corona lua
LOCAL_LDLIBS := -llog -lz


ifeq ($(TARGET_ARCH),arm)
LOCAL_CFLAGS+= -D_ARM_ASSEM_
endif

LOCAL_ARM_MODE := arm

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
#	LOCAL_CFLAGS := $(LOCAL_CFLAGS) -DHAVE_NEON=0
#	LOCAL_ARM_NEON  := true	
endif

include $(BUILD_SHARED_LIBRARY)
