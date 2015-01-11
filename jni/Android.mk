LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE:=um
LOCAL_SRC_FILES:=um.c
LOCAL_C_INCLUDES:= $(LOCAL_PATH)/include
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
include $(BUILD_EXECUTABLE)