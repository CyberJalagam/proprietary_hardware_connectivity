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

#ifndef MTK_HOSTAPD_HIDL_HIDL_MANAGER_H
#define MTK_HOSTAPD_HIDL_HIDL_MANAGER_H

#include <map>
#include <string>

#include <vendor/mediatek/hardware/wifi/hostapd/2.0/IHostapdCallback.h>

#include "hostapd.h"

extern "C" {
#include "utils/common.h"
#include "utils/includes.h"
}

namespace vendor {
namespace mediatek {
namespace hardware {
namespace wifi {
namespace hostapd {
namespace V2_0 {
namespace implementation {
using namespace vendor::mediatek::hardware::wifi::hostapd::V2_0;
using ::android::hardware::Return;

/**
 * HidlManager is responsible for managing the lifetime of all
 * hidl objects created by hostapd. This is a singleton
 * class which is created by the hostapd core and can be used
 * to get references to the hidl objects.
 */
class HidlManager
{
public:
    static HidlManager *getInstance();
    static void destroyInstance();

    // Methods called from hostapd core.
    void notifyApStaAuthorized(struct hostapd_iface *iface, const u8 *sta_addr);
    void notifyApStaDeauthorized(struct hostapd_iface *iface, const u8 *sta_addr);

    // Methods called from hidl objects.
    int addHostapdCallbackHidlObject(
        const android::sp<IHostapdCallback> &callback);

private:
    HidlManager() = default;
    ~HidlManager() = default;
    HidlManager(const HidlManager &) = default;
    HidlManager &operator=(const HidlManager &) = default;

    void removeHostapdCallbackHidlObject(
        const android::sp<IHostapdCallback> &callback);

    void callWithEachHostapdCallback(
        const std::function<android::hardware::Return<void>(
            android::sp<IHostapdCallback>)> &method);

    // Singleton instance of this class.
    static HidlManager *instance_;

    // Callback registered for the main hidl service object.
    std::vector<android::sp<IHostapdCallback>> hostapd_callbacks_;
};

}  // namespace implementation
}  // namespace V2_0
}  // namespace hostapd
}  // namespace wifi
}  // namespace hardware
}  // namespace mediatek
}  // namespace vendor
#endif  // MTK_HOSTAPD_HIDL_HIDL_MANAGER_H