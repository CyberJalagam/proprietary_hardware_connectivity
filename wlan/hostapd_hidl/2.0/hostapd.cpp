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

#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <android-base/file.h>
#include <android-base/stringprintf.h>

#include "hostapd.h"
#include "hidl/1.0/hidl_return_util.h"

extern "C"
{
#include "utils/eloop.h"
#include "ap/mtk_iface.h"
}

#include "hidl_manager.h"
extern "C" int hostapd_ctrl_iface_set(struct hostapd_data *hapd, char *cmd);

// The HIDL implementation for hostapd creates a hostapd.conf dynamically for
// each interface. This file can then be used to hook onto the normal config
// file parsing logic in hostapd code.  Helps us to avoid duplication of code
// in the HIDL interface.
// TOOD(b/71872409): Add unit tests for this.
namespace {
constexpr char kConfFileNameFmt[] = "/data/vendor/wifi/hostapd/hostapd_%s.conf";
constexpr char kAcceptMacFileNameFmt[] = "/data/vendor/wifi/hostapd/accept_mac_update.conf";

using android::base::StringPrintf;
using android::base::WriteStringToFile;
using android::hardware::wifi::hostapd::V1_0::IHostapd;

std::string WriteHostapdConfig(
    const std::string& interface_name, const std::string& config)
{
    const std::string file_path =
        StringPrintf(kConfFileNameFmt, interface_name.c_str());
    if (WriteStringToFile(
        config, file_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,
        getuid(), getgid())) {
        return file_path;
    }
    // Diagnose failure
    int error = errno;
    wpa_printf(
        MSG_ERROR, "Cannot write hostapd config to %s, error: %s",
        file_path.c_str(), strerror(error));
    struct stat st;
    int result = stat(file_path.c_str(), &st);
    if (result == 0) {
        wpa_printf(
            MSG_ERROR, "hostapd config file uid: %d, gid: %d, mode: %d",
            st.st_uid, st.st_gid, st.st_mode);
    } else {
        wpa_printf(
            MSG_ERROR,
            "Error calling stat() on hostapd config file: %s",
            strerror(errno));
    }
    return "";
}

std::string CreateHostapdConfig(
    const IHostapd::IfaceParams& iface_params,
    const vendor::mediatek::hardware::wifi::hostapd::V2_0::IHostapd::NetworkParams& nw_params)
{
    if (nw_params.ssid.size() >
        static_cast<uint32_t>(
        IHostapd::ParamSizeLimits::SSID_MAX_LEN_IN_BYTES)) {
        wpa_printf(
            MSG_ERROR, "Invalid SSID size: %zu", nw_params.ssid.size());
        return "";
    }
    if ((nw_params.encryptionType != IHostapd::EncryptionType::NONE) &&
        (nw_params.pskPassphrase.size() <
         static_cast<uint32_t>(
             IHostapd::ParamSizeLimits::
             WPA2_PSK_PASSPHRASE_MIN_LEN_IN_BYTES) ||
         nw_params.pskPassphrase.size() >
         static_cast<uint32_t>(
             IHostapd::ParamSizeLimits::
             WPA2_PSK_PASSPHRASE_MAX_LEN_IN_BYTES))) {
        wpa_printf(
            MSG_ERROR, "Invalid psk passphrase size: %zu",
            nw_params.pskPassphrase.size());
        return "";
    }

    // SSID string
    std::stringstream ss;
    ss << std::hex;
    ss << std::setfill('0');
    for (uint8_t b : nw_params.ssid) {
        ss << std::setw(2) << static_cast<unsigned int>(b);
    }
    const std::string ssid_as_string = ss.str();

    // Encryption config string
    std::string encryption_config_as_string;
    switch (nw_params.encryptionType) {
    case IHostapd::EncryptionType::NONE:
        // no security params
        break;
    case IHostapd::EncryptionType::WPA:
        encryption_config_as_string = StringPrintf(
            "wpa=3\n"
            "wpa_pairwise=TKIP CCMP\n"
            "wpa_passphrase=%s",
            nw_params.pskPassphrase.c_str());
        break;
    case IHostapd::EncryptionType::WPA2:
        encryption_config_as_string = StringPrintf(
            "wpa=2\n"
            "rsn_pairwise=CCMP\n"
            "wpa_passphrase=%s",
            nw_params.pskPassphrase.c_str());
        break;
    default:
        wpa_printf(MSG_ERROR, "Unknown encryption type");
        return "";
    }

    std::string channel_config_as_string;
    if (iface_params.channelParams.enableAcs) {
        channel_config_as_string = StringPrintf(
            "channel=0\n"
            "acs_exclude_dfs=%d",
            iface_params.channelParams.acsShouldExcludeDfs);
    } else {
        channel_config_as_string = StringPrintf(
            "channel=%d", iface_params.channelParams.channel);
    }

    // Hw Mode String
    std::string hw_mode_as_string;
    std::string ht_cap_vht_oper_chwidth_as_string;
    switch (iface_params.channelParams.band) {
    case IHostapd::Band::BAND_2_4_GHZ:
        hw_mode_as_string = "hw_mode=g";
        break;
    case IHostapd::Band::BAND_5_GHZ:
        hw_mode_as_string = "hw_mode=a";
        if (iface_params.channelParams.enableAcs) {
            ht_cap_vht_oper_chwidth_as_string =
                "ht_capab=[HT40+]\n"
                "vht_oper_chwidth=1";
        }
        break;
    case IHostapd::Band::BAND_ANY:
        hw_mode_as_string = "hw_mode=any";
        if (iface_params.channelParams.enableAcs) {
            ht_cap_vht_oper_chwidth_as_string =
                "ht_capab=[HT40+]\n"
                "vht_oper_chwidth=1";
        }
        break;
    default:
        wpa_printf(MSG_ERROR, "Invalid band");
        return "";
    }

    return StringPrintf(
        "interface=%s\n"
        "driver=nl80211\n"
        "ctrl_interface=/data/vendor/wifi/hostapd/ctrl\n"
        // ssid2 signals to hostapd that the value is not a literal value
        // for use as a SSID.  In this case, we're giving it a hex
        // std::string and hostapd needs to expect that.
        "ssid2=%s\n"
        "%s\n"
        "ieee80211n=%d\n"
        "ieee80211ac=%d\n"
        "%s\n"
        "%s\n"
        "ignore_broadcast_ssid=%d\n"
        "wowlan_triggers=any\n"
        "%s\n"
        "max_num_sta=%d\n"
        "macaddr_acl=%s\n"
        "accept_mac_file=%s\n"
        "device_name=AndroidAP\n"
        "manufacturer=MediaTek Inc.\n"
        "model_name=MTK Wireless Model\n"
        "model_number=66xx\n"
        "serial_number=1.0\n"
        "device_type=10-0050F204-5\n",
        iface_params.ifaceName.c_str(), ssid_as_string.c_str(),
        channel_config_as_string.c_str(),
        iface_params.hwModeParams.enable80211N ? 1 : 0,
        iface_params.hwModeParams.enable80211AC ? 1 : 0,
        hw_mode_as_string.c_str(), ht_cap_vht_oper_chwidth_as_string.c_str(),
        nw_params.isHidden ? 1 : 0, encryption_config_as_string.c_str(),
        nw_params.maxNumSta, nw_params.macAddrAcl.c_str(), kAcceptMacFileNameFmt);
}

std::string WriteAcceptMacConfig(const std::string& accept_mac_content)
{
    const std::string file_path = StringPrintf(kAcceptMacFileNameFmt);
    wpa_printf(MSG_INFO, "%s:\n%s", file_path.c_str(), accept_mac_content.c_str());
    if (WriteStringToFile(
        accept_mac_content, file_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP,
        getuid(), getgid())) {
        return file_path;
    }
    // Diagnose failure
    int error = errno;
    wpa_printf(
        MSG_ERROR, "Cannot write accept mac config to %s, error: %s",
        file_path.c_str(), strerror(error));
    struct stat st;
    int result = stat(file_path.c_str(), &st);
    if (result == 0) {
        wpa_printf(
            MSG_ERROR, "accept mac config file uid: %d, gid: %d, mode: %d",
            st.st_uid, st.st_gid, st.st_mode);
    } else {
        wpa_printf(
            MSG_ERROR,
            "Error calling stat() on accept mac config file: %s",
            strerror(errno));
    }
    return "";
}
}  // namespace

namespace vendor {
namespace mediatek {
namespace hardware {
namespace wifi {
namespace hostapd {
namespace V2_0 {
namespace implementation {
using android::hardware::wifi::hostapd::V1_0::implementation::hidl_return_util::call;
using android::hardware::wifi::hostapd::V1_0::HostapdStatus;
using android::hardware::wifi::hostapd::V1_0::HostapdStatusCode;

Hostapd::Hostapd(struct hapd_interfaces* interfaces) : interfaces_(interfaces)
{}

Return<void> Hostapd::addAccessPoint(
    const android::hardware::wifi::hostapd::V1_0::IHostapd::IfaceParams& iface_params,
    const vendor::mediatek::hardware::wifi::hostapd::V2_0::IHostapd::NetworkParams& nw_params,
    addAccessPoint_cb _hidl_cb)
{
    return call(
        this, &Hostapd::addAccessPointInternal, _hidl_cb, iface_params,
        nw_params);
}

Return<void> Hostapd::registerCallback(
    const sp<IHostapdCallback>& cb,
    registerCallback_cb _hidl_cb)
{
    return call(
        this, &Hostapd::registerCallbackInternal, _hidl_cb, cb);
}

Return<void> Hostapd::blockClient(
    const hidl_string& deviceAddress,
    blockClient_cb _hidl_cb)
{
    return call(
        this, &Hostapd::blockClientInternal, _hidl_cb, deviceAddress);
}

Return<void> Hostapd::unblockClient(
    const hidl_string& deviceAddress,
    unblockClient_cb _hidl_cb)
{
    return call(
        this, &Hostapd::unblockClientInternal, _hidl_cb, deviceAddress);
}

Return<void> Hostapd::updateAllowedList(
    const hidl_string& filePath,
    updateAllowedList_cb _hidl_cb)
{
    return call(
        this, &Hostapd::updateAllowedListInternal, _hidl_cb, filePath);
}

Return<void> Hostapd::setAllDevicesAllowed(
    bool enable,
    setAllDevicesAllowed_cb _hidl_cb)
{
    return call(
        this, &Hostapd::setAllDevicesAllowedInternal, _hidl_cb, enable);
}

HostapdStatus Hostapd::addAccessPointInternal(
    const android::hardware::wifi::hostapd::V1_0::IHostapd::IfaceParams& iface_params,
    const vendor::mediatek::hardware::wifi::hostapd::V2_0::IHostapd::NetworkParams& nw_params)
{
    if (hostapd_get_iface(interfaces_, iface_params.ifaceName.c_str())) {
        wpa_printf(
            MSG_ERROR, "Interface %s already present",
            iface_params.ifaceName.c_str());
        return {HostapdStatusCode::FAILURE_IFACE_EXISTS, ""};
    }
    WriteAcceptMacConfig(nw_params.acceptMacFileContent);
    const auto conf_params = CreateHostapdConfig(iface_params, nw_params);
    if (conf_params.empty()) {
        wpa_printf(MSG_ERROR, "Failed to create config params");
        return {HostapdStatusCode::FAILURE_ARGS_INVALID, ""};
    }
    const auto conf_file_path =
        WriteHostapdConfig(iface_params.ifaceName, conf_params);
    if (conf_file_path.empty()) {
        wpa_printf(MSG_ERROR, "Failed to write config file");
        return {HostapdStatusCode::FAILURE_UNKNOWN, ""};
    }
    std::string add_iface_param_str = StringPrintf(
        "%s config=%s", iface_params.ifaceName.c_str(),
        conf_file_path.c_str());
    std::vector<char> add_iface_param_vec(
        add_iface_param_str.begin(), add_iface_param_str.end() + 1);
    if (hostapd_add_iface(interfaces_, add_iface_param_vec.data()) < 0) {
        wpa_printf(
            MSG_ERROR, "Adding interface %s failed",
            add_iface_param_str.c_str());
        return {HostapdStatusCode::FAILURE_UNKNOWN, ""};
    }
    struct hostapd_data* iface_hapd =
        hostapd_get_iface(interfaces_, iface_params.ifaceName.c_str());
    WPA_ASSERT(iface_hapd != nullptr && iface_hapd->iface != nullptr);
    if (hostapd_enable_iface(iface_hapd->iface) < 0) {
        wpa_printf(
            MSG_ERROR, "Enabling interface %s failed",
            iface_params.ifaceName.c_str());
        return {HostapdStatusCode::FAILURE_UNKNOWN, ""};
    }

    ifname_ = iface_params.ifaceName;

    return {HostapdStatusCode::SUCCESS, ""};
}

HostapdStatus Hostapd::registerCallbackInternal(
    const sp<IHostapdCallback>& cb)
{
    HidlManager* hidl_manager = HidlManager::getInstance();
    if (!hidl_manager ||
        hidl_manager->addHostapdCallbackHidlObject(cb)) {
        return {HostapdStatusCode::FAILURE_UNKNOWN, ""};
    }
    return {HostapdStatusCode::SUCCESS, ""};
}

HostapdStatus Hostapd::blockClientInternal(const std::string& deviceAddress)
{
    struct hostapd_iface *iface = retrieveIfacePtr();
    char reply[4096] = {0};
    const int reply_size = 4096;
    std::string cmd_str = "STA-BLOCK " + deviceAddress;
    if (hostapd_driver_cmd(iface->bss[0], (char *)cmd_str.c_str(), reply, reply_size) < 0) {
        return {HostapdStatusCode::FAILURE_UNKNOWN, ""};
    }
    return {HostapdStatusCode::SUCCESS, ""};
}

HostapdStatus Hostapd::unblockClientInternal(const std::string& deviceAddress)
{
    struct hostapd_iface *iface = retrieveIfacePtr();
    char reply[4096] = {0};
    const int reply_size = 4096;
    std::string cmd_str = "STA-UNBLOCK " + deviceAddress;
    if (hostapd_driver_cmd(iface->bss[0], (char *)cmd_str.c_str(), reply, reply_size) < 0) {
        return {HostapdStatusCode::FAILURE_UNKNOWN, ""};
    }
    return {HostapdStatusCode::SUCCESS, ""};
}

HostapdStatus Hostapd::updateAllowedListInternal(const std::string& acceptMacFileContent)
{
    WriteAcceptMacConfig(acceptMacFileContent);
    struct hostapd_iface *iface = retrieveIfacePtr();
    std::string cmd_str = "accept_mac_file " + StringPrintf(kAcceptMacFileNameFmt);
    if (hostapd_ctrl_iface_set(iface->bss[0], (char *)cmd_str.c_str()) < 0) {
        return {HostapdStatusCode::FAILURE_UNKNOWN, ""};
    }
    return {HostapdStatusCode::SUCCESS, ""};
}

HostapdStatus Hostapd::setAllDevicesAllowedInternal(bool enable)
{
    struct hostapd_iface *iface = retrieveIfacePtr();
    std::string cmd_str = "macaddr_acl ";// + (enable ? "0" : "1");
    if (enable) {
        cmd_str += "0";
    } else {
        cmd_str += "1";
    }
    if (hostapd_ctrl_iface_set(iface->bss[0], (char *)cmd_str.c_str()) < 0) {
        return {HostapdStatusCode::FAILURE_UNKNOWN, ""};
    }
    return {HostapdStatusCode::SUCCESS, ""};
}

/**
 * Retrieve the underlying |hostapd| struct
 * pointer for this iface.
 * If the underlying iface is removed, then all RPC method calls on this object
 * will return failure.
 */
hostapd_iface *Hostapd::retrieveIfacePtr()
{
    return hostapd_get_iface(interfaces_, ifname_.c_str())->iface;
}
}  // namespace implementation
}  // namespace V2_0
}  // namespace hostapd
}  // namespace wifi
}  // namespace hardware
}  // namespace mediatek
}  // namespace vendor
