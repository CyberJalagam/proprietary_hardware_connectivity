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

#include "hostapd.h"
#include "hidl_manager.h"

extern "C"
{
#include "mtk_hidl.h"
#include "utils/common.h"
}

using vendor::mediatek::hardware::wifi::hostapd::V2_0::IHostapd;
using vendor::mediatek::hardware::wifi::hostapd::V2_0::implementation::Hostapd;
using vendor::mediatek::hardware::wifi::hostapd::V2_0::implementation::HidlManager;

// This file is a bridge between the hostapd code written in 'C' and the HIDL
// interface in C++. So, using "C" style static globals here!
static android::sp<IHostapd> service;

int mtk_hostapd_hidl_init(struct hapd_interfaces *interfaces)
{
    if (!HidlManager::getInstance())
        goto err;

    service = new Hostapd(interfaces);
    if (!service)
        goto err;
    if (service->registerAsService() != android::NO_ERROR)
        goto err;
    return 0;
err:
    mtk_hostapd_hidl_deinit();
    return -1;
}

void mtk_hostapd_hidl_deinit()
{
    service.clear();
    HidlManager::destroyInstance();
}

void mtk_hostapd_hidl_notify_ap_sta_authorized(struct hostapd_iface *iface, const u8 *sta_addr)
{
    if (!iface || !sta_addr)
        return;

    wpa_printf(
        MSG_INFO,
        "Notifying AP STA authorized to hidl control " MACSTR,
        MAC2STR(sta_addr));

    HidlManager *hidl_manager = HidlManager::getInstance();
    if (!hidl_manager)
        return;

    hidl_manager->notifyApStaAuthorized(iface, sta_addr);
}

void mtk_hostapd_hidl_notify_ap_sta_deauthorized(struct hostapd_iface *iface, const u8 *sta_addr)
{
    if (!iface || !sta_addr)
        return;

    wpa_printf(
        MSG_INFO,
        "Notifying AP STA deauthorized to hidl control " MACSTR,
        MAC2STR(sta_addr));

    HidlManager *hidl_manager = HidlManager::getInstance();
    if (!hidl_manager)
        return;

    hidl_manager->notifyApStaDeauthorized(iface, sta_addr);
}
