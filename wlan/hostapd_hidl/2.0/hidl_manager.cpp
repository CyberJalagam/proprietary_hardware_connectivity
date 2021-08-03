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

#include <algorithm>
#include <regex>

#include "hidl_manager.h"

namespace {

/**
 * Add callback to the corresponding list after linking to death on the
 * corresponding hidl object reference.
 */
template <class CallbackType>
int registerForDeathAndAddCallbackHidlObjectToList(
    const android::sp<CallbackType> &callback,
    const std::function<void(const android::sp<CallbackType> &)>
    &on_hidl_died_fctor,
    std::vector<android::sp<CallbackType>> &callback_list)
{
    callback_list.push_back(callback);
    return 0;
}
}  // namespace

namespace vendor {
namespace mediatek {
namespace hardware {
namespace wifi {
namespace hostapd {
namespace V2_0 {
namespace implementation {

HidlManager *HidlManager::instance_ = NULL;

HidlManager *HidlManager::getInstance()
{
    if (!instance_)
        instance_ = new HidlManager();
    return instance_;
}

void HidlManager::destroyInstance()
{
    if (instance_)
        delete instance_;
    instance_ = NULL;
}

void HidlManager::notifyApStaAuthorized(struct hostapd_iface *iface, const u8 *sta_addr)
{
    if (!iface || !sta_addr)
        return;

    callWithEachHostapdCallback(std::bind(
        &IHostapdCallback::onStaAuthorized, std::placeholders::_1,
        sta_addr));
}

void HidlManager::notifyApStaDeauthorized(struct hostapd_iface *iface, const u8 *sta_addr)
{
    if (!iface || !sta_addr)
        return;

    callWithEachHostapdCallback(std::bind(
        &IHostapdCallback::onStaDeauthorized, std::placeholders::_1,
        sta_addr));
}

/**
 * Add a new |IHostapdCallback| hidl object reference to our
 * global callback list.
 *
 * @param callback Hidl reference of the |IHostapdCallback| object.
 *
 * @return 0 on success, 1 on failure.
 */
int HidlManager::addHostapdCallbackHidlObject(
    const android::sp<IHostapdCallback> &callback)
{
    // Register for death notification before we add it to our list.
    auto on_hidl_died_fctor = std::bind(
        &HidlManager::removeHostapdCallbackHidlObject, this,
        std::placeholders::_1);
    return registerForDeathAndAddCallbackHidlObjectToList<
        IHostapdCallback>(
        callback, on_hidl_died_fctor, hostapd_callbacks_);
}

/**
 * Removes the provided |IHostapdCallback| hidl object reference
 * from our global callback list.
 *
 * @param callback Hidl reference of the |IHostapdCallback| object.
 */
void HidlManager::removeHostapdCallbackHidlObject(
    const android::sp<IHostapdCallback> &callback)
{
    hostapd_callbacks_.erase(
        std::remove(
        hostapd_callbacks_.begin(), hostapd_callbacks_.end(),
        callback),
        hostapd_callbacks_.end());
}

/**
 * Helper function to invoke the provided callback method on all the
 * registered |IHostapdCallback| callback hidl objects.
 *
 * @param method Pointer to the required hidl method from
 * |IHostapdCallback|.
 */
void HidlManager::callWithEachHostapdCallback(
    const std::function<Return<void>(android::sp<IHostapdCallback>)> &method)
{
    for (const auto &callback : hostapd_callbacks_) {
        if (!method(callback).isOk()) {
            wpa_printf(MSG_ERROR, "Failed to invoke HIDL callback");
        }
    }
}
}  // namespace implementation
}  // namespace V2_0
}  // namespace hostapd
}  // namespace wifi
}  // namespace hardware
}  // namespace mediatek
}  // namespace vendor
