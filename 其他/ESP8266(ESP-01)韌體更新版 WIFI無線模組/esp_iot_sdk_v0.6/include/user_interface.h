/*
 *  Copyright (C) 2013 -2014  Espressif System
 *
 */

#ifndef __USER_INTERFACE_H__
#define __USER_INTERFACE_H__

#include "ip_addr.h"
#include "queue.h"
#include "user_config.h"

#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

#define UPGRADE_FW_BIN1_SEC     112
#define UPGRADE_FW_BIN2_SEC     14

#define UPGRADE_FW_BIN1_SEC_NUM     14  // 32KB + 24KB
#define UPGRADE_FW_BIN2_SEC_NUM     48  // 192KB

#define UPGRADE_FW_BIN1         0x00
#define UPGRADE_FW_BIN2         0x01

#define UPGRADE_FLAG_IDLE       0x00
#define UPGRADE_FLAG_START      0x01
#define UPGRADE_FLAG_FINISH     0x02

void system_upgrade_init(uint8 bin);
void system_upgrade_deinit(uint8 bin);
uint8 system_upgrade_flag_check(uint8 bin);
void system_upgrade_flag_set(uint8 bin, uint8 flag);
bool system_upgrade(uint8 bin, uint8 *data, uint16 len);

void system_restore(void);
void system_restart(void);
void system_deep_sleep(uint32 time_in_us);

void system_timer_reinit(void);
uint32 system_get_time(void);

#define NULL_MODE       0x00
#define STATION_MODE    0x01
#define SOFTAP_MODE     0x02
#define STATIONAP_MODE  0x03

struct station_config {
    uint8 ssid[32];
    uint8 password[64];
};

typedef enum _auth_mode {
    AUTH_OPEN           = 0,
    AUTH_WEP,
    AUTH_WPA_PSK,
    AUTH_WPA2_PSK,
    AUTH_WPA_WPA2_PSK
} AUTH_MODE;

struct softap_config {
    uint8 ssid[32];
    uint8 password[64];
    uint8 channel;
    uint8 authmode;
};

struct bss_info {
    STAILQ_ENTRY(bss_info)     next;

    u8 bssid[6];
    u8 ssid[32];
    u8 channel;
    s8 rssi;
    u8 authmode;
};

typedef struct _scaninfo {
    STAILQ_HEAD(, bss_info) *pbss;
    struct espconn *pespconn;
    u8 totalpage;
    u8 pagenum;
    u8 page_sn;
    u8 data_cnt;
} scaninfo;


bool load_user_param(uint16 sec, void *param, uint16 size);
bool save_user_param(uint16 sec, void *param, uint16 size);
bool restore_user_param(uint16 sec);

uint8 wifi_get_opmode(void);
void wifi_set_opmode(uint8 opmode);

typedef void (* scan_done_cb_t)(void *arg, STATUS status);

void wifi_station_get_config(struct station_config *config);
void wifi_station_set_config(struct station_config *config);

void wifi_station_connect(void);
void wifi_station_disconnect(void);
void wifi_station_scan(scan_done_cb_t cb);

void wifi_softap_get_config(struct softap_config *config);
void wifi_softap_set_config(struct softap_config *config);
void wifi_softap_set_ssid_hidden(bool hidden);

#define STATION_IF      0x00
#define SOFTAP_IF       0x01

void wifi_get_ip_info(uint8 if_index, struct ip_info *info);
void wifi_get_macaddr(uint8 if_index, uint8 *macaddr);

/** Get the absolute difference between 2 u32_t values (correcting overflows)
 * 'a' is expected to be 'higher' (without overflow) than 'b'. */
#define ESP_U32_DIFF(a, b) (((a) >= (b)) ? ((a) - (b)) : (((a) + ((b) ^ 0xFFFFFFFF) + 1)))

#endif
