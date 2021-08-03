LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

ifeq ($(findstring MT6627_FM,$(MTK_FM_CHIP)),MT6627_FM)
LOCAL_CFLAGS+= \
    -DMT6627_FM
endif
ifeq ($(findstring MT6625_FM,$(MTK_FM_CHIP)),MT6625_FM)
LOCAL_CFLAGS+= \
    -DMT6627_FM
endif
ifeq ($(findstring MT6580_FM,$(MTK_FM_CHIP)),MT6580_FM)
LOCAL_CFLAGS+= \
    -DMT6580_FM
endif
ifeq ($(findstring MT0633_FM,$(MTK_FM_CHIP)),MT0633_FM)
LOCAL_CFLAGS+= \
    -DMT0633_FM
endif
ifeq ($(findstring MT6630_FM,$(MTK_FM_CHIP)),MT6630_FM)
LOCAL_CFLAGS+= \
    -DMT6630_FM
endif
ifeq ($(findstring MT6631_FM,$(MTK_FM_CHIP)),MT6631_FM)
LOCAL_CFLAGS+= \
    -DMT6631_FM
endif
ifeq ($(findstring MT6632_FM,$(MTK_FM_CHIP)),MT6632_FM)
LOCAL_CFLAGS+= \
    -DMT6632_FM
endif


LOCAL_MODULE_ONWER := mtk

#LOCAL_INCLUDE_MTK_GLOBAL_CONFIGS := no
LOCAL_INIT_RC := fm_hidl_service.rc

LOCAL_MODULE := fm_hidl_service
LOCAL_PROPRIETARY_MODULE := true

#LOCAL_C_INCLUDES += \
#    $(LOCAL_PATH)/mtk_socket_utils/inc \

LOCAL_SRC_FILES := \
    fm_hidl_service.cpp \
    fmr_core.cpp \
    common.cpp \
    fmr_err.cpp \
    custom.cpp


LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libdl \
    libbase \
    libhardware \
    libbinder \
    libhidlbase \
    libhidltransport \
    libutils \
    vendor.mediatek.hardware.fm@1.0

#include $(BUILD_SHARED_LIBRARY)

include $(MTK_EXECUTABLE)
