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

#ifndef MTK_HOSTAPD_HIDL_HIDL_H
#define MTK_HOSTAPD_HIDL_HIDL_H

#ifdef __cplusplus
extern "C"
{
#endif  // _cplusplus
#include "ap/hostapd.h"

/**
 * This is the hidl RPC interface entry point to the hostapd core.
 * This initializes the hidl driver & IHostapd instance.
 */
int mtk_hostapd_hidl_init(struct hapd_interfaces *interfaces);
void mtk_hostapd_hidl_deinit();

#ifdef CONFIG_CTRL_IFACE_HIDL
void mtk_hostapd_hidl_notify_ap_sta_authorized(struct hostapd_iface *iface, const u8 *sta_addr);
void mtk_hostapd_hidl_notify_ap_sta_deauthorized(struct hostapd_iface *iface, const u8 *sta_addr);
#else   // CONFIG_CTRL_IFACE_HIDL
static inline void mtk_hostapd_hidl_notify_ap_sta_authorized(
    struct hostapd_iface *iface, const u8 *sta_addr)
{
}
static inline void mtk_hostapd_hidl_notify_ap_sta_deauthorized(
    struct hostapd_iface *iface, const u8 *sta_addr)
{
}
#endif  // CONFIG_CTRL_IFACE_HIDL

#ifdef __cplusplus
}
#endif  // _cplusplus

#endif  // MTK_HOSTAPD_HIDL_HIDL_H
