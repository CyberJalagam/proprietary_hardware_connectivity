/*
 * Copyright (C) 2016-2020 China IWNCOMM Co., Ltd. All rights reserved.
 *
 * This module is wapi cert parse function.
 * this function parse cert.
 *
 * Authors:
 * <iwncomm@iwncomm.com>
 *
 * History:
 * pi yongping 06/06/2016 v1.0 add by piyp
 *
 * Related documents:
 * wapi_cert.h
 *
 * Description:
 *
 */

int
Is_P12_Cert (unsigned char *user_cert , int user_cert_len );
int
Check_Asue_Asu_Cert (unsigned char *user_cert , int user_cert_len ,
        unsigned char *pri_key , int pri_key_len ,
        unsigned char *as_cert , int as_cert_len );
int
Parse_P12_Cert (unsigned char *inp12 , int len , char *password ,
        unsigned char *usrcert , unsigned short *usrcert_len ,
        unsigned char *privatekey , unsigned short *privatekey_len );

int
Get_Asue_Cert_Info (unsigned char *usrcert , int certlen ,char *certinfo);

