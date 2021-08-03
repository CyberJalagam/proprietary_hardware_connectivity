/*
 * hidl interface for wpa_supplicant daemon
 * Copyright (c) 2004-2016, Jouni Malinen <j@w1.fi>
 * Copyright (c) 2004-2016, Roshan Pius <rpius@google.com>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef MTK_WPA_SUPPLICANT_HIDL_HIDL_H
#define MTK_WPA_SUPPLICANT_HIDL_HIDL_H

#ifdef _cplusplus
extern "C" {
#endif  // _cplusplus

/**
 * This is the hidl RPC interface entry point to the wpa_supplicant core.
 * This initializes the hidl driver & HidlManager instance and then forwards
 * all the notifcations from the supplicant core to the HidlManager.
 */
struct wpas_hidl_priv;
struct wpa_global;

void mtk_wpas_hidl_init(struct wpa_global *global);
void mtk_wpas_hidl_deinit();

#ifdef CONFIG_CTRL_IFACE_HIDL
int mtk_wpas_hidl_register_interface(struct wpa_supplicant *wpa_s);
int mtk_wpas_hidl_unregister_interface(struct wpa_supplicant *wpa_s);
int mtk_wpas_hidl_register_network(
    struct wpa_supplicant *wpa_s, struct wpa_ssid *ssid);
int mtk_wpas_hidl_unregister_network(
    struct wpa_supplicant *wpa_s, struct wpa_ssid *ssid);
void mtk_wpas_hidl_notify_wnm_ess_disassoc_imminent_notice(
    struct wpa_supplicant *wpa_s, u32 pmf_enabled, u32 reauth_delay, const char *url);
#else   // CONFIG_CTRL_IFACE_HIDL
static inline int mtk_wpas_hidl_register_interface(struct wpa_supplicant *wpa_s)
{
    return 0;
}
static inline int mtk_wpas_hidl_unregister_interface(struct wpa_supplicant *wpa_s)
{
    return 0;
}
static inline int mtk_wpas_hidl_register_network(
    struct wpa_supplicant *wpa_s, struct wpa_ssid *ssid)
{
    return 0;
}
static inline int mtk_wpas_hidl_unregister_network(
    struct wpa_supplicant *wpa_s, struct wpa_ssid *ssid)
{
    return 0;
}
static inline void mtk_wpas_hidl_notify_wnm_ess_disassoc_imminent_notice(
    struct wpa_supplicant *wpa_s, u32 pmf_enabled, u32 reauth_delay, const char *url)
{
}
#endif  // CONFIG_CTRL_IFACE_HIDL

#ifdef _cplusplus
}
#endif  // _cplusplus

#endif  // MTK_WPA_SUPPLICANT_HIDL_HIDL_H
