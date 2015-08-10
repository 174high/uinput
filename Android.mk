LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= \
    uinput.c

LOCAL_MODULE := uinput

include $(BUILD_EXECUTABLE)
