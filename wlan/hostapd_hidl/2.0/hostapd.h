/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2016. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef MTK_HOSTAPD_HIDL_SUPPLICANT_H
#define MTK_HOSTAPD_HIDL_SUPPLICANT_H

#include <string>

#include <android-base/macros.h>

#include <android/hardware/wifi/hostapd/1.0/IHostapd.h>
#include <vendor/mediatek/hardware/wifi/hostapd/2.0/IHostapd.h>

extern "C"
{
#include "utils/common.h"
#include "utils/includes.h"
#include "utils/wpa_debug.h"
#include "ap/hostapd.h"
}

namespace vendor {
namespace mediatek {
namespace hardware {
namespace wifi {
namespace hostapd {
namespace V2_0 {
namespace implementation {
using android::hardware::hidl_string;
using android::hardware::Return;
using android::sp;
using android::hardware::wifi::hostapd::V1_0::HostapdStatus;
using android::hardware::wifi::hostapd::V1_0::IHostapd;

/**
 * Implementation of the hostapd hidl object. This hidl
 * object is used core for global control operations on
 * hostapd.
 */
class Hostapd : public vendor::mediatek::hardware::wifi::hostapd::V2_0::IHostapd
{
public:
    Hostapd(hapd_interfaces* interfaces);
    ~Hostapd() override = default;

    // Hidl methods exposed.
    Return<void> addAccessPoint(
        const android::hardware::wifi::hostapd::V1_0::IHostapd::IfaceParams& iface_params,
        const vendor::mediatek::hardware::wifi::hostapd::V2_0::IHostapd::NetworkParams& nw_params,
        addAccessPoint_cb _hidl_cb) override;
    Return<void> registerCallback(
        const sp<IHostapdCallback>& cb,
        registerCallback_cb _hidl_cb) override;
    Return<void> blockClient(
        const hidl_string& deviceAddress,
        blockClient_cb _hidl_cb) override;
    Return<void> unblockClient(
        const hidl_string& deviceAddress,
        unblockClient_cb _hidl_cb) override;
    Return<void> updateAllowedList(
        const hidl_string& acceptMacFileContent,
        updateAllowedList_cb _hidl_cb) override;
    Return<void> setAllDevicesAllowed(
        bool enable,
        setAllDevicesAllowed_cb _hidl_cb) override;

private:
    // Corresponding worker functions for the HIDL methods.
    HostapdStatus addAccessPointInternal(
        const android::hardware::wifi::hostapd::V1_0::IHostapd::IfaceParams& iface_params,
        const vendor::mediatek::hardware::wifi::hostapd::V2_0::IHostapd::NetworkParams& nw_params);
    HostapdStatus registerCallbackInternal(const sp<IHostapdCallback>& cb);
    HostapdStatus blockClientInternal(const std::string& deviceAddress);
    HostapdStatus unblockClientInternal(const std::string& deviceAddress);
    HostapdStatus updateAllowedListInternal(const std::string& acceptMacFileContent);
    HostapdStatus setAllDevicesAllowedInternal(bool enable);
    struct hostapd_iface *retrieveIfacePtr();

    // Name of the iface this hidl object controls
    std::string ifname_;

    // Raw pointer to the global structure maintained by the core.
    struct hapd_interfaces* interfaces_;

    DISALLOW_COPY_AND_ASSIGN(Hostapd);
};
}  // namespace implementation
}  // namespace V2_0
}  // namespace hostapd
}  // namespace wifi
}  // namespace hardware
}  // namespace mediatek
}  // namespace vendor

#endif  // MTK_HOSTAPD_HIDL_SUPPLICANT_H
