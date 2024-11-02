#include "typesdef.h"
#include "list.h"
#include "dev.h"
#include "devid.h"
#include "osal/string.h"
#include "osal/semaphore.h"
#include "osal/mutex.h"
#include "osal/irq.h"
#include "osal/task.h"
#include "osal/sleep.h"
#include "osal/timer.h"
#include "osal/string.h"

#include "lib/lwip/include/lwip/sockets.h"
#include "lib/lwip/include/lwip/netdb.h"
#include "lib/atcmd/libatcmd.h"
#include "osal/string.h"
#include "osal/sleep.h"
#include "lib/sysvar.h"
#include "osal/mutex.h"
#include "osal/semaphore.h"
#include "osal/task.h"
#include "lib/wnb/libwnb.h"
#include "lib/syscfg.h"
#include "lib/lmac/lmac.h"

typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr    SOCKADDR;
typedef unsigned long      DWORD;
struct dsleep_psdata *dsleep_data;

#define UDP_KALIVE_HEARTBEAT_DATA      "UALIVE:HB"
#define UDP_KALIVE_HEARTBEAT_RESP_DATA "UALIVE:HB"
#define UDP_KALIVE_WAKEUP_DATA         "UALIVE:WK"
#define UDP_KALIVE_ONLINE              "UALIVE:OL"
#define UDP_KALIVE_TESTDATA            "UALIVE:TD"
#define UDP_KALIVE_GOTOSLEEP           "UALIVE:GS"
#define UALIVE_FRM(d, s) (memcmp(d, s, 9) == 0)
#define MAC2STR(a) (a)[0]&0xff, (a)[1]&0xff, (a)[2]&0xff, (a)[3]&0xff, (a)[4]&0xff, (a)[5]&0xff

struct udp_kalive_mgr {
    int sock;
    int port;
    int dev_cnt;
    char id[18];
    SOCKADDR_IN svr;
    char testdata[1400];
    char recvbuf[2048];
    int hb, hb_tmo;
} udp_klive;

static int udp_kalive_init(int port)
{
    int sock = -1;
    int addr_len = sizeof(SOCKADDR_IN);
    SOCKADDR_IN local_addr;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("create udp socket error:[%d:%s]\n", errno, strerror(errno));
        return -1;
    }

    memset(&local_addr, 0, addr_len);
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    local_addr.sin_port = htons(port);
    if (bind(sock, (SOCKADDR *)&local_addr, addr_len) < 0) {
        printf("udp bind error:[%d:%s]\n", errno, strerror(errno));
        closesocket(sock);
        return -1;
    }
    return sock;
}

static int udp_kalive_send(SOCKADDR_IN *dest, char *data, int len)
{
    int addr_len = sizeof(SOCKADDR_IN);
    return sendto(udp_klive.sock, data, len, 0, (SOCKADDR *)dest, addr_len); 
}

static int udp_kalive_recv(int sock, SOCKADDR_IN *dest, char *data, int len, int tmo)
{
    int ret = 0;
    fd_set rfd;
    struct timeval timeout;
    uint32 addr_len = sizeof(SOCKADDR_IN);

    FD_ZERO(&rfd);
    FD_SET(sock, &rfd);
    timeout.tv_sec  = 0;
    timeout.tv_usec = tmo * 1000;
    ret = select(sock + 1, &rfd, NULL, NULL, &timeout);
    if (FD_ISSET(sock, &rfd)) {
        ret = recvfrom(sock, data, len, 0, (SOCKADDR *)dest, &addr_len);
    } else {
        ret = 0;
    }
    return ret;
}

static void udp_kalive_dev_send(char *action, int param1, int param2, int param3)
{
    int len = 0;
    char data[64];
    memset(data, 0, 64);
    len = sprintf(data, "%s-%s", action, udp_klive.id);
    if (param1) { len += sprintf(data + len, ",%d", param1); }
    if (param2) { len += sprintf(data + len, ",%d", param2); }
    if (param3) { len += sprintf(data + len, ",%d", param3); }
    udp_kalive_send(&udp_klive.svr, data, strlen(data));
}

static void udp_kalive_send_hearbeat()
{
    udp_kalive_dev_send(UDP_KALIVE_HEARTBEAT_DATA, udp_klive.hb, 0, 0);
}

static void udp_kalive_send_online()
{
    udp_kalive_dev_send(UDP_KALIVE_ONLINE, 0, 0, 0);
}

static void udp_kalive_send_testdata()
{
    SOCKADDR_IN svr;
    memcpy(&svr, &udp_klive.svr, sizeof(SOCKADDR_IN));
    svr.sin_port = htons(udp_klive.port + 1);
    udp_kalive_send(&svr, udp_klive.testdata, sizeof(udp_klive.testdata));
}

static void udp_kalive_config_hbresp()
{
    
    uint8 data[32];
    memset(data, 0, 32);
    sprintf((char *)data, "%s-%s", UDP_KALIVE_HEARTBEAT_DATA, udp_klive.id);
    int32 data_len = os_strlen(data);
    if(data_len > PSALIVE_HEARTBEAT_RESP_SIZE-2) data_len = PSALIVE_HEARTBEAT_RESP_SIZE-2;
    if(dsleep_data){
        put_unaligned_le16(data_len, dsleep_data->hb_resp);
        os_memcpy(dsleep_data->hb_resp+2, data, data_len);
        dsleep_data->hbresp_len = data_len;
        printf("heartbeat resp set success!\r\n");
    }
}

static void udp_kalive_config_heartbeat(in_addr_t ip, int port, int hb_int, int hb_tmo)
{

    if(dsleep_data){
        dsleep_data->ip     = htonl(ip);//0x0a0a0a10
        dsleep_data->port   = (uint32)port;
        dsleep_data->period = (uint16)hb_int;
        dsleep_data->hb_tmo = (uint16)hb_tmo;
        printf("heartbeat data set success!\r\n");
    }
}

static void udp_kalive_config_wkdata()
{
    uint8 data[32];
    memset(data, 0, 32);
    sprintf((char *)data, "%s-%s", UDP_KALIVE_WAKEUP_DATA, udp_klive.id);
    int32 data_len = os_strlen(data);
    if(data_len > PSALIVE_HEARTBEAT_RESP_SIZE-2) data_len = PSALIVE_HEARTBEAT_RESP_SIZE-2;
    if(dsleep_data){
        put_unaligned_le16(data_len, dsleep_data->wk_data);
        os_memcpy(dsleep_data->wk_data+2, data, data_len);
        dsleep_data->wkdata_len = data_len;
        printf("wake up data set success!\r\n");
    }
}

static int udp_kalive_detect_module(char *ifname)
{
    int ret = 1;
    return ret;
}

static int dsleep_test(char* ip, uint32 port, uint32 hb, uint32 hb_tmo)
{
    int ret = 0;
    SOCKADDR_IN from;
    
    int sleep_status = 0;
    int i = 0;
    int tick_sec = 0;
    int svr_alive = 0;
    in_addr_t ipaddr;
    
    struct wireless_nb *wnb = sysvar(SYSVAR_ID_WIRELESS_NB);
    dsleep_data = lmac_dsleep_psdata();
    sprintf(udp_klive.id, MACSTR, MAC2STR(wnb->cfg->addr));

    ipaddr = inet_addr(ip);
    udp_klive.port = (int)port;
    udp_klive.hb = (int)hb;
    udp_klive.hb_tmo = (int)hb_tmo;

    memset(&udp_klive.svr, 0, sizeof(SOCKADDR_IN));
    udp_klive.svr.sin_family = AF_INET;
    udp_klive.svr.sin_addr.s_addr = ipaddr;
    udp_klive.svr.sin_port = htons(udp_klive.port);
    printf("ps server:%d:%d\r\n", ipaddr, udp_klive.port);
    
    udp_klive.sock = udp_kalive_init(0);
    if (udp_klive.sock < 0) {
        return -1;
    }

__detect:
    while (udp_kalive_detect_module("hg0") == -1) {
        os_sleep(100 * 1000);
    }

    printf("AH module detect ...\r\n");
    udp_kalive_send_online();
    udp_kalive_send_online();
    sprintf(udp_klive.testdata, "%s-%s", UDP_KALIVE_TESTDATA, udp_klive.id);
    udp_kalive_config_heartbeat(udp_klive.svr.sin_addr.s_addr, udp_klive.port, udp_klive.hb, udp_klive.hb_tmo);
    udp_kalive_config_hbresp();
    udp_kalive_config_wkdata();
    sleep_status  = 0;
    svr_alive = 0;
    tick_sec = os_jiffies();

    while (1) {
        ret = udp_kalive_recv(udp_klive.sock, &from, udp_klive.recvbuf, 2048, 100);
        if (ret > 0 && strncmp(udp_klive.recvbuf, "UALIVE:", 7) == 0) {
            udp_klive.recvbuf[ret] = 0;
            if (UALIVE_FRM(udp_klive.recvbuf, UDP_KALIVE_GOTOSLEEP)) {
                sleep_status = 1;
                udp_kalive_send_hearbeat();
                printf("go sleep ...\r\n");
            } else if (UALIVE_FRM(udp_klive.recvbuf, UDP_KALIVE_HEARTBEAT_RESP_DATA) && (sleep_status == 1)) {
                sleep_status = 2;
                atcmd_recv((uint8 *)"AT+DSLEEP=1",os_strlen("AT+DSLEEP=1"));
            } else if (UALIVE_FRM(udp_klive.recvbuf, UDP_KALIVE_ONLINE)) {
                svr_alive = 10;
            }
        } else {
            if (udp_kalive_detect_module("hg0") == -1) {
                goto __detect;
            }

            if (os_jiffies() - tick_sec >= 1000) {
                tick_sec = os_jiffies();
                if (sleep_status == 1) {
                    udp_kalive_send_hearbeat();
                } else if (sleep_status == 2) {
                    atcmd_recv((uint8 *)"AT+DSLEEP=1",os_strlen("AT+DSLEEP=1"));
                } else if (sleep_status == 0) {
                    udp_kalive_send_online();
                    if (svr_alive > 0) { svr_alive--; }
                }
            }

            if (!sleep_status && svr_alive) {
                for (i = 0; i < 40; i++) {
                    udp_kalive_send_testdata();
                }
            }
        }
    }
}

static int32 wnb_atcmd_dsleep_test_hdl(const char *cmd, uint8 *data, int32 len)
{
    if (atcmd_args_num() > 0 && atcmd_args(0)[1] != '?') {
        
        if (atcmd_args_num() == 1){
            sys_cfgs.dsleep_test_cfg[0] = inet_addr(atcmd_args(0));
            atcmd_ok;
        }
        else if (atcmd_args_num() == 4){
            sys_cfgs.dsleep_test_cfg[0] = inet_addr(atcmd_args(0));
            sys_cfgs.dsleep_test_cfg[1] = os_atoi(atcmd_args(1));
            sys_cfgs.dsleep_test_cfg[2] = os_atoi(atcmd_args(2));
            sys_cfgs.dsleep_test_cfg[3] = os_atoi(atcmd_args(3));
            atcmd_ok;
        }
        else {
            atcmd_error;
        }
        syscfg_save();
        mcu_reset();
    }
    return 0;
}

struct os_task dsleeptest_task;

static void dsleep_test_task(void *arg)
{
    if (sys_cfgs.dsleep_test_cfg[0] != 0 && sys_cfgs.dsleep_test_cfg[1] == 0){
        dsleep_test(inet_ntoa(sys_cfgs.dsleep_test_cfg[0]), 60002, 10, 60);
    }
    else {
        dsleep_test(inet_ntoa(sys_cfgs.dsleep_test_cfg[0]), sys_cfgs.dsleep_test_cfg[1], sys_cfgs.dsleep_test_cfg[2], sys_cfgs.dsleep_test_cfg[3]);
    }
}

struct wnb_config wnbcfg;

void dsleep_test_init()
{
    atcmd_register("AT+DSLEEP_TEST", wnb_atcmd_dsleep_test_hdl, NULL);
    wnbcfg.psmode=2;
}

void dsleep_test_start()
{
    if (sys_cfgs.dsleep_test_cfg[0] != 0) {
        OS_TASK_INIT("dsleeptest", &dsleeptest_task, dsleep_test_task, 0, OS_TASK_PRIORITY_LOW, 1024);
    }
}
