### Hidl service library ###
########################
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libhostapd_mtk_hidl
LOCAL_VENDOR_MODULE := true
# Apply hostapd's C & CPP Flags
LOCAL_CPPFLAGS := $(MTK_HOSTAPD_HIDL_CPPFLAGS)
LOCAL_CFLAGS := $(MTK_HOSTAPD_HIDL_CFLAGS)

SUPP_PATH := external/wpa_supplicant_8
LOCAL_C_INCLUDES := $(SUPP_PATH)/src $(SUPP_PATH)/hostapd $(SUPP_PATH)/src/common $(SUPP_PATH)/src/utils
MTK_HIDL_INTERFACE_VERSION := 2.0
LOCAL_SRC_FILES := \
    $(MTK_HIDL_INTERFACE_VERSION)/hidl.cpp \
    $(MTK_HIDL_INTERFACE_VERSION)/hidl_manager.cpp \
    $(MTK_HIDL_INTERFACE_VERSION)/hostapd.cpp
LOCAL_SHARED_LIBRARIES := \
    vendor.mediatek.hardware.wifi.hostapd@2.0 \
    libbase \
    libhidlbase \
    libhidltransport \
    libutils \
    liblog
LOCAL_EXPORT_C_INCLUDE_DIRS := \
    $(LOCAL_PATH)/$(MTK_HIDL_INTERFACE_VERSION)
include $(BUILD_STATIC_LIBRARY)