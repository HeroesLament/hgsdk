#include "project_config.h"
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
#include "hal/gpio.h"
#include "hal/watchdog.h"
#include "hal/sysaes.h"
#include "hal/timer_device.h"
#include "lib/common.h"
#include "lib/syscfg.h"
#include "lib/sysheap.h"
#include "lib/skb/skb.h"
#include "lib/skb/skb_list.h"
#include "lib/mac_bus.h"
#include "lib/lmac/lmac.h"
#include "lib/lmac/lmac_host.h"
#include "lib/wnb/libwnb.h"
#include "lib/xmodem.h"
#include "lib/atcmd/libatcmd.h"
#include <lib/lwip/include/lwip/err.h>
#include <lib/lwip/include/lwip/sockets.h>
#include <lib/lwip/include/lwip/netdb.h>
#include <lib/lwip/include/lwip/sys.h>
#include <lib/lwip/include/lwip/ip_addr.h>
#include <lib/lwip/include/lwip/tcpip.h>
#include <lib/lwip/include/lwip/icmp.h>
#include <lib/lwip/include/lwip/mem.h>
#include <lib/lwip/include/lwip/raw.h>
#include <lib/lwip/include/lwip/inet_chksum.h>
#include <lib/lwip/include/lwip/inet.h>
#include <lib/lwip/include/lwip/ip.h>
#include <lib/lwip/include/netif/ethernetif.h>
#include <lib/dhcpd/dhcpd.h>

#ifdef DEEP_SLEEP

#define SYS_PAIR_KEY      PB_1
//#define SYS_PAIR_LED      PB_3
#define SYS_PAIR_LED      PB_4
#define SYS_SIGNAL_LED1   PB_5
#define SYS_SIGNAL_LED2   PB_6
#define SYS_SIGNAL_LED3   PB_7
//#define SYS_CONN_LED_IO   PB_3
#define SYS_CONN_LED_IO   PB_4
#define SYS_ROLE_KEY      PB_2

#else

#define SYS_PAIR_KEY      PA_7
#define SYS_PAIR_LED      PA_6
#define SYS_SIGNAL_LED1   PA_9
#define SYS_SIGNAL_LED2   PA_31
#define SYS_SIGNAL_LED3   PA_30
#define SYS_CONN_LED_IO   PA_6
#define SYS_ROLE_KEY      PA_8
#endif

static struct os_task wdog_task;

#ifdef PAIR_KEY_ENABLE
static struct os_task sys_pairkey_task;
#endif
#ifdef ROLE_IO_ENABLE
static struct os_task sys_rolekey_task;
#endif

#ifndef TIMER1_ENABLE_MS
#define TIMER1_ENABLE_MS      0
#endif
#ifndef TIMER3_ENABLE_MS
#define TIMER3_ENABLE_MS      0
#endif

/*default values*/
struct wnb_config wnbcfg = {
    .role = WNB_STA_ROLE_SLAVE,
    .chan_list = {9080, 9160, 9240},
    .chan_cnt = 3,
    .bss_bw   = 8,
    .pri_chan = 3,
    .tx_mcs   = 0xff,
    .encrypt  = 1,
    .forward  = 1,
    .max_sta  = WNB_STA_COUNT,
    .acs_enable = 1,
    .bss_max_idle = 300,
    .beacon_int = IOT_FMAC_BEACONINT,
    .heartbeat_int = IOT_FMAC_HEARINT,
    .dtim_period = 2,
    .pa_pwrctrl_dis = DSLEEP_PAPWRCTL_DIS,
    .dcdc13 = DC_DC_1_3V,
    .dupfilter = 1,    
};

static uint8 sysdbg_heap = 0;
static uint8 sysdbg_top = 0;
static uint8 sysdbg_irq = 0;
static uint8 sysdbg_wnb = 0;
static uint8 sysdbg_lmac = 1;
static uint8 pair_waitting = 0;

static int32 sys_atcmd_sysdbg_hdl(const char *cmd, uint8 *data, int32 len)
{
    char *arg = atcmd_args(0);
    if (atcmd_args_num() == 2) {
        if (os_strcasecmp(arg, "heap") == 0) {
            sysdbg_heap = os_atoi(atcmd_args(1));
        }
        if (os_strcasecmp(arg, "top") == 0) {
            sysdbg_top = os_atoi(atcmd_args(1));
        }
        if (os_strcasecmp(arg, "irq") == 0) {
            sysdbg_irq = os_atoi(atcmd_args(1));
        }
        if (os_strcasecmp(arg, "wnb") == 0) {
            sysdbg_wnb = os_atoi(atcmd_args(1));
        }
        if (os_strcasecmp(arg, "lmac") == 0) {
            sysdbg_lmac = os_atoi(atcmd_args(1));
        }
        atcmd_ok;
    } else {
        atcmd_error;
    }
    return 0;
}

static int32 syscfg_save_def(void)
{
    int ret = RET_ERR;
    if (sys_cfgs.magic_num != SYSCFG_MAGIC) {
        // set syscfg default value
        // save wnbcfg (actully total syscfg) at last
        ret = wnb_save_cfg(&wnbcfg, 1);
        os_printf("save default syscfg %s\r\n", ret ? "Fail" : "OK");
    }
    return ret;
}

#if 1 /*tcp test code*/
struct os_task tcptest_task;
in_addr_t tcptest_ip = 0;
uint16  tcptest_port = 0;
int32   tcpmode = 0;

static void tcp_test_client(void *arg)
{
    int sock = -1;
    int ret  = -1;
    //int optval = 1;
    struct sockaddr_in server_addr;
    uint8 *txbuf = os_malloc(1400);

    os_memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = tcptest_ip;
    server_addr.sin_port        = htons(tcptest_port);

    ASSERT(txbuf);
    os_printf("tcp test start ...\r\n");
    while (1) {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            os_printf("create socket fail\n");
            os_sleep(1);
            continue;
        }

        //ret = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
        ret = connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));
        if (ret < 0) {
            os_printf("connect fail, ret:<%d>!\n", ret);
            os_sleep(1);
            closesocket(sock);
            continue;
        }

        while (1) {
            ret = send(sock, txbuf, 1400, 0);
            if (ret < 0) {
                os_printf("send fail, ret:<%d>!\n", ret);
                break;
            }
            os_sleep_ms(12);
        }
        closesocket(sock);
    }
}

static void tcp_test_server(void *arg)
{
    int s = -1;
    int sock = -1;
    int ret  = -1;
    int addrlen = sizeof(struct sockaddr_in);
    struct sockaddr_in addr;
    uint8 *tcpbuf = os_malloc(1500);
    uint32 rxlen = 0;
    uint32 lasttick;

    os_memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(tcptest_port);

    ASSERT(tcpbuf);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        os_printf("create socket fail\n");
        os_sleep(1);
        return;
    }

    ret = bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr));
    if (ret < 0) {
        os_printf("bind fail, ret:<%d>!\n", ret);
        closesocket(sock);
        return;
    }
    
    if (listen(sock, 20) < 0) {
        os_printf("tcp listen error\n");
        closesocket(sock);
        return;
    }

    os_printf("tcp test start ...\r\n");
    while (1) {
        s = accept(sock, (struct sockaddr *)&addr, (socklen_t *)&addrlen);
        if (s != -1) {
            os_printf("accept new client from %s:%d\r\n", inet_ntoa(addr.sin_addr.s_addr), os_ntohs(addr.sin_port));
            rxlen = 0;
            lasttick = os_jiffies();        
            while (1) {
                ret = recv(s, tcpbuf, 1500, 0);
                if (ret < 0) {
                    os_printf("recv fail, close socket <%d>!\n", ret);
                    break;
                }
                rxlen += ret;
                if(TIME_AFTER(os_jiffies(), lasttick+os_msecs_to_jiffies(1000))){
                    os_printf("tcp test: %dKB/s\r\n", rxlen/1024);
                    rxlen = 0;
                    lasttick = os_jiffies();        
                }
            }
            closesocket(s);
        }
    }
    closesocket(sock);
}

static int32 sys_atcmd_tcptest_hdl(const char *cmd, uint8 *data, int32 len)
{
    if (atcmd_args_num() >= 2) {
        if(tcptest_ip){
            atcmd_printf("+ERROR: tcp test busy\r\n");
            return 0;
        }

        tcpmode      = (atcmd_args_num()==3?(os_strcmp(atcmd_args(2),"s")==0):0);
        tcptest_ip   = inet_addr(atcmd_args(0));
        tcptest_port = os_atoi(atcmd_args(1));
        if (tcptest_port == 0) { 
            tcptest_port = 60001; 
        }
        
        os_printf("tcp test: %s:%s, mode:%s\r\n", atcmd_args(0), atcmd_args(1), tcpmode?"Server":"Client");
        if(tcpmode){
            OS_TASK_INIT("tcptest", &tcptest_task, tcp_test_server, 0, OS_TASK_PRIORITY_LOW, 1024);            
        }
        else{
            if (tcptest_ip == IPADDR_NONE) {
                atcmd_error;
                return 0;
            }
            OS_TASK_INIT("tcptest", &tcptest_task, tcp_test_client, 0, OS_TASK_PRIORITY_LOW, 1024);
        }
        atcmd_ok;
    } else {
        atcmd_error;
    }
    return 0;
}
#endif

#if 1 /*ping code*/

static int ping_recv(int sockfd, void *buf, int len, int flags, struct sockaddr *src_addr,
                     socklen_t *addrlen, unsigned int timeout_ms)
{
    int             ret = 0;
    struct timeval  to;
    fd_set          fdset;
    FD_ZERO(&fdset);
    FD_SET(sockfd, &fdset);
    to.tv_sec = timeout_ms / 1000;
    to.tv_usec = (timeout_ms % 1000) * 1000;
    ret = lwip_select(sockfd + 1, &fdset, NULL, NULL, timeout_ms == 0 ? NULL : &to);

    if (ret > 0) {
        if (FD_ISSET(sockfd, &fdset)) {
            ret = lwip_recvfrom(sockfd, buf, len, flags, src_addr, addrlen);

            if (ret <= 0) {
                ret = -1;
            }
        } else {
            ret = -1;
        }
    }

    return ret;
}

static void lwip_ping(char *ip_domain, int pktsize, unsigned int send_times)
{
    int isdomain = 0;
    int seqno = 0;
    int sock = -1;
    uint32 send_cnt = 0;
    uint32 recv_cnt = 0;
    uint32 timeout_ms = 3000;
    uint32 ping_tick = 0;
    uint32 recv_tick = 0;
    struct sockaddr_in from;
    struct sockaddr_in to;
    struct icmp_echo_hdr *echo;
    int ipaddr = inet_addr(ip_domain);
    socklen_t addr_len  = sizeof(struct sockaddr_in);
    int buff_len  = sizeof(struct icmp_echo_hdr) + pktsize;
    char *recvbuf = NULL;

    if (ipaddr == INADDR_NONE) {
        struct hostent *host = lwip_gethostbyname(ip_domain);
        if (host == NULL) {
            os_printf("can not resolve domain name:%s\n", ip_domain);
            return;
        }

        ipaddr = *(int *)host->h_addr;
        isdomain = 1;
    }

    if ((sock = lwip_socket(AF_INET, SOCK_RAW, IP_PROTO_ICMP)) < 0) {
        os_printf("create socket failed!\n");
        return;
    }

    echo = os_malloc(buff_len);
    if (echo == NULL) {
        os_printf("malloc failed, request to malloc size:0x%x\n", buff_len);
        lwip_close(sock);
        return ;
    }

    recvbuf = os_malloc(buff_len + sizeof(struct ip_hdr));

    if (recvbuf == NULL) {
        os_printf("malloc failed, request to malloc size:0x%x\n", buff_len);
        os_free(echo);
        lwip_close(sock);
        return ;
    }

    to.sin_addr.s_addr = ipaddr;
    to.sin_family = AF_INET;
    to.sin_len = sizeof(to);

    if (isdomain) {
        os_printf("\n\nPinging %s[%s] with %d bytes of data:\n", ip_domain, inet_ntoa(ipaddr), pktsize);
    } else {
        os_printf("\n\nPinging %s with %d bytes of data:\n", ip_domain, pktsize);
    }

    while (send_cnt++ < send_times || send_times == 0) {
        ICMPH_TYPE_SET(echo, ICMP_ECHO);
        ICMPH_CODE_SET(echo, 0);
        echo->chksum = 0;
        echo->id     = 0xAFAF;
        echo->seqno  = htons(++seqno);
        echo->chksum = inet_chksum(echo, buff_len);
        ping_tick = os_jiffies();

        if (lwip_sendto(sock, (char *)echo, buff_len, 0, (const struct sockaddr *)&to, addr_len) > 0) {
            if (ping_recv(sock, recvbuf, buff_len + sizeof(struct ip_hdr), 0, (struct sockaddr *)&from, &addr_len, timeout_ms) >
                (int)(sizeof(struct ip_hdr) + sizeof(struct icmp_echo_hdr))) {
                recv_tick = os_jiffies();
                struct ip_hdr *iphdr = (struct ip_hdr *)recvbuf;
                struct icmp_echo_hdr *iecho = (struct icmp_echo_hdr *)(recvbuf + (IPH_HL(iphdr) * 4));

                if ((iecho->id == 0xAFAF) && (iecho->seqno == echo->seqno)) {
                    recv_cnt++;
                    os_printf("Reply from %s: bytes=%d time:%dms TTL=255\n",
                              inet_ntoa(from.sin_addr.s_addr), pktsize, recv_tick - ping_tick);
                }
            } else {
                os_printf("Request timed out.\n");
            }
        } else {
            os_printf("Ping %s error!!\n", inet_ntoa(ipaddr));
        }
        os_sleep_ms(1000);
    }

    os_printf("----------------------------------------------------------\n"\
              "Ping statistics for %s:\n"\
              "Packets: Sent = %d, Received = %d, Lost = %d (%d%% loss)\n",
              inet_ntoa(ipaddr), send_times, recv_cnt, (send_times - recv_cnt),
              (send_times - recv_cnt) * 100 / send_times);
    lwip_close(sock);
    os_free(echo);
    os_free(recvbuf);
}

static int32 sys_atcmd_ping_hdl(const char *cmd, uint8 *data, int32 len)
{
    if (atcmd_args_num() >= 1) {
        char *ip_domain = atcmd_args(0);
        unsigned int send_times = ((atcmd_args_num()==1 || os_strcmp(atcmd_args(1),"")==0) ? 5 : os_atoi(atcmd_args(1)));
        int pktsize = (atcmd_args_num()==3 ? os_atoi(atcmd_args(2)) : 32);
        atcmd_ok;
        lwip_ping(ip_domain, pktsize, send_times);
    } else {
        atcmd_error;
    }
    return 0;
}
#endif

static int32 atcmd_ip_hdl(const char *cmd, uint8 *data, int32 len)
{
    char *arg = atcmd_args(0);
    struct netif *netif = NULL;
    netif = netif_find("w0");
    if (netif == NULL) {
        atcmd_printf("ip is not exist\r\n");
        return 0;
    }
    if(atcmd_args_num() > 0 && arg[0] != '?') {
        netif->ip_addr.addr = inet_addr(atcmd_args(0));
        atcmd_ok;
    } else {
        atcmd_resp("%s",inet_ntoa(netif->ip_addr.addr));
    }

    return 0;
}

#if 1 /*test code*/
static void dsleep_test()
{
    extern void dsleep_test_init();
    extern void dsleep_test_start();
    dsleep_test_init();
    dsleep_test_start();
}
#endif

static void watchdog_task(void *args)
{
    struct watchdog_device *wd = (struct watchdog_device *)dev_get(HG_WDTV1_DEVID);
    ASSERT(wd);
    watchdog_device_open(wd, 0);
    watchdog_device_start(wd, 5);
    while (1) {
        watchdog_device_feed(wd);
        os_sleep(2);
    }
}

#ifdef PAIR_KEY_ENABLE
static int32 sys_pair_key_val(void)
{
    int32 i, v;
    int32 v0 = 0, v1 = 0;

    for (i = 0; i < 50; i++) {
        v = gpio_get_val(SYS_PAIR_KEY);
        if (v) { v1++; }
        else  { v0++; }
    }
    return (v1 > v0 ? 1 : 0);
}

static void sys_pair_key_task(void *args)
{
    int32 new_val, last_val, defval;

    defval  = 1;
    last_val = sys_pair_key_val();

    os_printf("key default: %d\r\n", defval);
    while (1) {
        os_sleep_ms(100);
        new_val = sys_pair_key_val();
        if (new_val != last_val) {
            if (new_val == defval) {
                wnb_pairing(0);
            } else {
                wnb_pairing(1);
            }
            last_val = new_val;
        }
    }
}

static void sys_pair_waiting(void)
{
    static uint8 waiting_led = 0;
    if (pair_waitting) {
        gpio_set_val(SYS_PAIR_LED, waiting_led ? 0 : 1);
        waiting_led = !waiting_led;
    }
}

#endif

#ifdef ROLE_IO_ENABLE
static void sys_role_key_task(void *args)
{
    int32 new_val, last_val;

    last_val = gpio_get_val(SYS_ROLE_KEY);

    while (1) {
        os_sleep_ms(100);
        new_val = gpio_get_val(SYS_ROLE_KEY);
        if (new_val != last_val) {
            os_printf("role key press:%d\r\n", new_val);
            mcu_reset();
        }
    }
}
#endif

#if (TIMER1_ENABLE_MS > 0)
static void timer1_cb(uint32 args)
{

}
static void timer1_init(uint32 tmo_ms, enum timer_type type, uint32 cb_data)
{
    struct timer_device *timer1 = (struct timer_device *)dev_get(HG_TIMER1_DEVID);
    timer_device_open(timer1, type, 0);
    timer_device_start(timer1, tmo_ms*1000, timer1_cb, cb_data);
}
#endif

#if (TIMER3_ENABLE_MS > 0)
static void timer3_cb(uint32 args)
{

}
static void timer3_init(uint32 tmo_ms, enum timer_type type, uint32 cb_data)
{
    struct timer_device *timer3 = (struct timer_device *)dev_get(HG_TIMER3_DEVID);
    timer_device_open(timer3, type, 0);
    timer_device_start(timer3, tmo_ms*1000, timer3_cb, cb_data);
}
#endif

#ifdef IOT_DEV_UART
void uart_iot_init(void);
void uart_send(uint8 *data, uint32 len);
void uart_receive(uint8 *data, int32 len)
{
    os_printf("uart recv data:%s\r\n", data);
}
#endif
#ifdef IOT_DEV_SPI
void spi_iot_master_init(void);
void spibus_master_write(uint8 *buff, int32 len);
void spi_receive(uint8 *data)
{
    os_printf("spi recv data:%s\r\n", data);
}
#endif

static void wnb_event_hdl(wnb_event evt, uint32 param1, uint32 param2)
{
    static int __pair_led_on = 0;
    switch (evt) {
        case WNB_EVENT_PAIR_START:
            pair_waitting = 1;
            gpio_set_val(SYS_PAIR_LED, 1);
            os_printf("wnb: start pairing ...\r\n");
            break;
        case WNB_EVENT_PAIR_SUCCESS:
            pair_waitting = 0;
            gpio_set_val(SYS_PAIR_LED, __pair_led_on ? 0 : 1);
            __pair_led_on = !__pair_led_on;
            os_printf("wnb: pairing success!\r\n");
            break;
        case WNB_EVENT_PAIR_DONE:
            pair_waitting = 0;
            gpio_set_val(SYS_PAIR_LED, 1);
            if (param2 > 0) {
                gpio_set_val(SYS_CONN_LED_IO, 0);
            } else {
                gpio_set_val(SYS_CONN_LED_IO, 1);
            }
            os_printf("wnb: pairing done, save wnb config!\r\n");
            if (wnb_save_cfg((struct wnb_config *)param1, 0)) {
                os_printf("save wnb config fail, try again!\r\n");
                wnb_save_cfg((struct wnb_config *)param1, 0);
            }
            break;
        case WNB_EVENT_RX_DATA:
            os_printf("wnb: rx customer data 0x%x, len:%d\r\n", param1, param2);
            break;
        case WNB_EVENT_CONNECT_START:
            os_printf("wnb: start connecting ...\r\n");
            break;
        case WNB_EVENT_CONNECTED:
            gpio_set_val(SYS_CONN_LED_IO, 0);
            os_printf("wnb: ["MACSTR"] connect success!\r\n", MAC2STR((uint8 *)param1));
            break;
        case WNB_EVENT_DISCONNECTED:
            if (param2 == 0) {
                gpio_set_val(SYS_CONN_LED_IO, 1);
                gpio_set_val(SYS_SIGNAL_LED3, 1);
                gpio_set_val(SYS_SIGNAL_LED2, 1);
                gpio_set_val(SYS_SIGNAL_LED1, 1);
            }
            os_printf("wnb: connection lost!\r\n");
            break;
        case WNB_EVENT_RSSI: /*param1: rssi, param2: sta count*/
            if (param2 == 0) { /*no sta*/
                gpio_set_val(SYS_SIGNAL_LED3, 1);
                gpio_set_val(SYS_SIGNAL_LED2, 1);
                gpio_set_val(SYS_SIGNAL_LED1, 1);
            } else if (param2 == 1) {/*AP: has 1 sta, or STA: connected to AP*/
                gpio_set_val(SYS_SIGNAL_LED3, ((int8)param1 >= -48) ? 0 : 1);
                gpio_set_val(SYS_SIGNAL_LED2, ((int8)param1 >= -60) ? 0 : 1);
                gpio_set_val(SYS_SIGNAL_LED1, ((int8)param1 >= -72) ? 0 : 1);
            } else { /*only for AP mode: more than 1 sta*/
                gpio_set_val(SYS_SIGNAL_LED3, 0);
                gpio_set_val(SYS_SIGNAL_LED2, 0);
                gpio_set_val(SYS_SIGNAL_LED1, 0);
            }
            break;
        case WNB_EVENT_EVM:
            if (param2 == 1) {
                if (((int8)param1) >= -20) {
                    gpio_set_val(SYS_SIGNAL_LED3, 1);
                }
                if (((int8)param1) >= -15) {
                    gpio_set_val(SYS_SIGNAL_LED2, 1);
                }
            }
            break;
        default:
            break;
    }
}

__init static void sys_atcmd_init(void)
{
    atcmd_init(ATCMD_UARTDEV);
    atcmd_register("AT+FWUPG", xmodem_fwupgrade_hdl, NULL);
    atcmd_register("AT+SYSDBG", sys_atcmd_sysdbg_hdl, NULL);
    atcmd_register("AT+SKBDUMP", atcmd_skbdump_hdl, NULL);
    atcmd_register("AT+IP", atcmd_ip_hdl, NULL);
#if 1 /*tcp test*/
    atcmd_register("AT+TCPTEST", sys_atcmd_tcptest_hdl, NULL);
#endif
#if 1 /*ping*/
    atcmd_register("AT+PING", sys_atcmd_ping_hdl, NULL);
#endif
}

__init static void sys_wifi_init(void)
{
    struct lmac_init_param lparam;
    struct wnb_init_param param;

#ifdef SKB_POOL_ENABLE
    skb_txpool_init((uint8 *)SKB_POOL_ADDR, (uint32)SKB_POOL_SIZE);
#endif

    os_memset(&param, 0, sizeof(param));
    os_memset(&lparam, 0, sizeof(lparam));
    lparam.rxbuf = WIFI_RX_BUFF_ADDR;
    lparam.rxbuf_size = WIFI_RX_BUFF_SIZE;
    lparam.tdma_buff = TDMA_BUFF_ADDR;
    lparam.tdma_buff_size = TDMA_BUFF_SIZE;
    param.ops = lmac_ah_init(&lparam);
    /*set default freq list*/
    if (module_efuse_info.module_type == MODULE_TYPE_750M) {
        wnbcfg.chan_list[0] = 7640;
        wnbcfg.chan_list[1] = 7720;
        wnbcfg.chan_list[2] = 7800;
    } else if (module_efuse_info.module_type == MODULE_TYPE_810M) {
        wnbcfg.chan_list[0] = 8060;
        wnbcfg.chan_list[1] = 8140;
        wnbcfg.chan_cnt = 2;
    } else if (module_efuse_info.module_type == MODULE_TYPE_860M) {
        wnbcfg.chan_list[0] = 8660;
        wnbcfg.chan_cnt = 1;
        wnbcfg.bss_bw   = 2;
    } else { // 915M case
        ;
    }

    wnb_load_cfg(&wnbcfg);
    ASSERT(wnbcfg.max_sta == WNB_STA_COUNT);

    wnb_load_factparam(&wnbcfg);
    sysctrl_efuse_mac_addr_calc(wnbcfg.addr);

    param.cb  = wnb_event_hdl;
    param.cfg = &wnbcfg;
    param.frm_type = WNB_FRM_TYPE_ETHER;
    param.if_type  = MAC_BUS_TYPE_NDEV;
#ifdef MACBUS_COMBI
    param.if_type  = MAC_BUS_TYPE_COMBI;
#endif
    param.hook_cnt = 10;

    wnb_init(&param);
    syscfg_save_def();
    wnb_connect(1);
}

__init void sys_dhcpd_start()
{
    struct dhcpd_param param;

    os_memset(&param, 0, sizeof(param));
    param.start_ip = inet_addr("10.10.10.100");
    param.end_ip   = inet_addr("10.10.10.200");
    param.netmask  = inet_addr("255.255.255.0");
    param.lease_time = 60;
    if(dhcpd_start("w0", &param)){
        os_printf("dhcpd start error\r\n");
    }
}

__init static void sys_network_init(void)
{
    struct netdev *ndev = (struct netdev *)dev_get(HG_WIFI_DEVID);

    tcpip_init(NULL, NULL);

    //add wifi interface
    if(ndev){
        lwip_netif_add(ndev, "w0", NULL, NULL, NULL);
        lwip_netif_set_default(ndev);
        lwip_netif_set_dhcp(ndev, 1);
    }

#ifdef LWIP_GMAC
    //add gmac interface
    ndev = (struct netdev *)dev_get(HG_GMAC_DEVID);
    if(ndev){
        char addr[6];
        os_memcpy(addr, wnbcfg.addr, 6);
        addr[5] += 1;
        netdev_set_macaddr(ndev, addr);
        lwip_netif_add(ndev, "e0", NULL, NULL, NULL);
        lwip_netif_set_default(ndev);
        lwip_netif_set_dhcp(ndev, 1);
    }
#endif

#ifdef DHCPD_ENABLE
    sys_dhcpd_start();
#endif
}

__init static void sys_keyled_init(void)
{
    gpio_set_dir(SYS_PAIR_LED, GPIO_DIR_OUTPUT);
    gpio_set_dir(SYS_SIGNAL_LED1, GPIO_DIR_OUTPUT);
    gpio_set_dir(SYS_SIGNAL_LED2, GPIO_DIR_OUTPUT);
    gpio_set_dir(SYS_SIGNAL_LED3, GPIO_DIR_OUTPUT);

    gpio_set_val(SYS_PAIR_LED, 1);
    gpio_set_val(SYS_SIGNAL_LED1, 1);
    gpio_set_val(SYS_SIGNAL_LED2, 1);
    gpio_set_val(SYS_SIGNAL_LED3, 1);

#ifdef PAIR_KEY_ENABLE
    gpio_set_dir(SYS_PAIR_KEY, GPIO_DIR_INPUT);
    gpio_set_mode(SYS_PAIR_KEY, GPIO_PULL_UP, GPIO_PULL_LEVEL_100K);
    OS_TASK_INIT("pair_key", &sys_pairkey_task, sys_pair_key_task, 0, OS_TASK_PRIORITY_NORMAL, 512);
#endif

#ifdef ROLE_IO_ENABLE
    gpio_set_dir(SYS_ROLE_KEY, GPIO_DIR_INPUT);
    gpio_set_mode(SYS_ROLE_KEY, GPIO_PULL_DOWN, GPIO_PULL_LEVEL_100K);
    wnbcfg.role = gpio_get_val(SYS_ROLE_KEY) ? WNB_STA_ROLE_MASTER : WNB_STA_ROLE_SLAVE;
    OS_TASK_INIT("role_key", &sys_rolekey_task, sys_role_key_task, 0, OS_TASK_PRIORITY_NORMAL, 512);
#endif
}

#if 1 // reduce code size.
void wnb_proc_cmd(struct wireless_nb *wnb){}
//int32 wnb_atdata_rx_proc(struct wireless_nb *wnb, struct wnb_rx_info *rxinfo){ return 0; };
//void wnb_atcmd_init(void){};
#endif

int main(void)
{
    int8 print_interval = 0;

    OS_TASK_INIT("watchdog", &wdog_task, watchdog_task, 0, OS_TASK_PRIORITY_NORMAL, 256);
    syscfg_init(&sys_cfgs, sizeof(sys_cfgs));
    
    os_printf("\r\n.....Taixin IOT SDK.....\r\n\n");
    
    sys_atcmd_init();
    sys_wifi_init();
    sys_network_init();
    sys_keyled_init();
    //dsleep_test();

    sysheap_collect_init();
    
#ifdef IOT_DEV_UART
    uart_iot_init();
#endif
#ifdef IOT_DEV_SPI
    spi_iot_master_init();
#endif
#if (TIMER1_ENABLE_MS > 0)
    uint32 cb_data = 1;
    timer1_init(TIMER1_ENABLE_MS, TIMER_TYPE_PERIODIC, cb_data);
#endif
#if (TIMER3_ENABLE_MS > 0)
    uint32 cb_data = 3;
    timer3_init(TIMER3_ENABLE_MS, TIMER_TYPE_PERIODIC, cb_data);
#endif

    while (1) {
        os_sleep(1);
#ifdef PAIR_KEY_ENABLE
        sys_pair_waiting();
#endif
        if (print_interval++ >= 5) {
            if (sysdbg_top) { cpu_loading_print(0); }
            if (sysdbg_heap) { sysheap_status(); }
            if (sysdbg_irq) { irq_status(); }
            if (sysdbg_wnb) { wnb_statistic_print(); }
            lmac_transceive_statics(sysdbg_lmac); 
            print_interval = 0;
        }
    }
}

