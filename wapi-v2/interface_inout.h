/*
 * Copyright (C) 2015-2020 China IWNCOMM Co., Ltd. All rights reserved.
 *
 * Contains data structures and function declarations
 * of wai interface
 *
 * Authors:
 * iwncomm
 *
 * History:
 * yucun tian  09/10/2015 v1.0 maintain the code.
 * yucun tian  07/20/2016 v1.1 fix for change code style.
 *
 * Related documents:
 * -GB/T 15629.11-2003/XG1-2006
 * -ISO/IEC 8802.11:1999,MOD
 *
*/

#ifndef _WAI_INTERFACE_EXP_H_
#define _WAI_INTERFACE_EXP_H_

#ifdef  __cplusplus
extern "C" {
#endif

/* struct */
typedef enum
{
	/* No WAPI */
	AUTH_TYPE_NONE_WAPI = 0,
	/* Certificate */
	AUTH_TYPE_WAPI_CERT,
	/* Pre-PSK */
	AUTH_TYPE_WAPI_PSK,
}AUTH_TYPE;

typedef enum
{
	/* ascii */
	KEY_TYPE_ASCII = 0,
	/* HEX */
	KEY_TYPE_HEX,
}KEY_TYPE;

typedef struct
{
	/* Authentication type */
	AUTH_TYPE authType;
	u8 reinit_cert_list;
	/* WAPI_CERT */
	char *cert_list;
	char *as_cert;
	char *asue_cert;
	/* WAPI_PSK Key type */
	KEY_TYPE kt;
	/* Key length */
	unsigned int kl;
	/* Key value */
	unsigned char kv[128];
}CNTAP_PARA;

typedef enum
{
	WAPI_AUTH_ERR,
	WAPI_ERR_NUM
} WAPI_LIB_ERR_CLASS;

typedef enum
{
	CONN_ASSOC = 0,
	CONN_DISASSOC,
}CONN_STATUS;

typedef struct
{
	unsigned char v[6];
	unsigned char pad[2];
}MAC_ADDRESS;

typedef void (*timeout_handler)(void *eloop_data, void *user_ctx);

struct wapi_cb_ctx {
	void *ctx; /* pointer to arbitrary upper level context */
	int mtu_len;

	/*face to mt592x*/
	int (*msg_send)(void *priv, const u8 *msg_in, int msg_in_len,
							 u8 *msg_out, int *msg_out_len);

	/*send output to wpa_ctrl*/
	void (*wpa_msg)(void *ctx, int level, const char *fmt, ...);

	void (*wapi_error)(void *ctx, int class, const char *fmt, ...);

	/*set wapi key*/
	int (*set_key)(void *ctx, int alg,
		       const u8 *addr, int key_idx, int set_tx,
		       const u8 *seq, size_t seq_len,
		       const u8 *key, size_t key_len);

	/*send WAI frame*/
	int (*ether_send)(void *ctx, const u8* pbuf, int length);

	/*set wpa_supplicant state*/
	void (*set_state)(void *ctx, int state);

	/*get wpa_supplicant state*/
	int (*get_state)(void *ctx);

	/*send deauthenticateion*/
	void (*deauthenticate)(void *ctx, int reason_code);

	/*set one-shot timer*/
	int  (*set_timer)(unsigned int secs, unsigned int usecs,
			   timeout_handler handler,
			   void *eloop_data, void *user_data);

	/*clear one-shot timer*/
	int  (*cancel_timer)(timeout_handler handler,
			 void *eloop_data, void *user_data);
};

/* libwapi interface functions */

int wapi_set_user(const CNTAP_PARA* pPar);

void wapi_set_msg(CONN_STATUS action, const MAC_ADDRESS* pBSSID, const MAC_ADDRESS* pLocalMAC, unsigned char *assoc_ie, unsigned char assoc_ie_len);

unsigned long wapi_set_rx_wai(const u8* pbuf, int length);

int wapi_lib_init(struct wapi_cb_ctx *wapi_cb);

int wapi_lib_exit();

int wapi_get_state(void);

unsigned int wapi_get_assoc_ie(unsigned char *assoc_ie);

int wapi_get_debug_level(void);

void wapi_set_debug_level(int level);

#ifdef  __cplusplus
}
#endif

#endif /* _WAI_INTERFACE_EXP_H_  */

