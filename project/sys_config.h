#ifndef __SYS_CONFIG_H__
#define __SYS_CONFIG_H__
#include <stdint.h>
#include "project_config.h"
/*
#define CONFIG_PATH(x) CONFIG_PATH1(x)
#define CONFIG_PATH1(x) #x
#define CONFIG_FILE(x) CONFIG_PATH(customer/x.h)
#define CUSTOMER meari
#include CONFIG_FILE(CUSTOMER)
*/
#define RTOS_ENABLE
#define OS_TASK_COUNT 20

#define SYSCFG_ENABLE
#define PROJECT_TYPE PRO_TYPE_IOTFMAC
//#define RSSID_34LED

/* SYS_KEY protect enable */
#define SYS_KEY_OPT_EN                  0

#define USING_TX_SQ
#define TDMA_DEBUG
#define RXSUBFRAME_EN       1
#define RXSUBFRAME_PERIOD   (5)
//#define DEEP_SLEEP

#define SYS_IRQ_STAT

#ifndef GMAC_BUFF
#define GMAC_BUFF (0)
#endif

#ifndef TDMA_BUFF_SIZE
#define TDMA_BUFF_SIZE    (32*4*100)
#endif
#ifndef SYS_HEAP_SIZE
#define  SYS_HEAP_SIZE (30*1024+GMAC_BUFF+1024*WNB_STA_COUNT)
#endif
#ifndef WIFI_RX_BUFF_SIZE
#define  WIFI_RX_BUFF_SIZE (128*1024-GMAC_BUFF-1024*WNB_STA_COUNT)
#endif
#if WIFI_RX_BUFF_SIZE < (64*1024)
#undef WIFI_RX_BUFF_SIZE
#define WIFI_RX_BUFF_SIZE (64*1024)
#endif

#ifndef IOT_FMAC_BEACONINT
#define IOT_FMAC_BEACONINT (500)
#endif

#ifndef IOT_FMAC_HEARINT
#define IOT_FMAC_HEARINT   (WNB_STA_COUNT*50)
#if IOT_FMAC_HEARINT < 500
#undef IOT_FMAC_HEARINT
#define IOT_FMAC_HEARINT 500
#endif
#endif

extern uint32_t userpool_start;
extern uint32_t userpool_end;
#define USER_POOL_START   (userpool_start)
#define USER_POOL_SIZE    ((userpool_end) - (userpool_start))
#define TDMA_BUFF_ADDR    (USER_POOL_START)
#define SYS_HEAP_START    (TDMA_BUFF_ADDR+TDMA_BUFF_SIZE)
#define WIFI_RX_BUFF_ADDR (SYS_HEAP_START+SYS_HEAP_SIZE)
#define SKB_POOL_ADDR     (WIFI_RX_BUFF_ADDR+WIFI_RX_BUFF_SIZE)
#define SKB_POOL_SIZE     (USER_POOL_START+USER_POOL_SIZE-SKB_POOL_ADDR)

#define K_EXT_PA
//#define K_iPA
#ifdef K_EXT_PA
#define K_VMODE_CNTL
#endif
#define K_SINGLE_PIN_SWITCH
//#define K_MIPI_IF

#define RF_PARA_FROM_NOR   //if open it to read parameter from nor in case efuse is not ready
                            //when efuse data is ready, close it

/*! Use GPIO to simulate the MII management interface                           */
#define HG_GMAC_IO_SIMULATION
#ifndef HG_GMAC_MDIO_PIN
#define HG_GMAC_MDIO_PIN PA_10
#endif
#ifndef HG_GMAC_MDC_PIN
#define HG_GMAC_MDC_PIN  PA_11
#endif

#define HGGPIO_DEFAULT_LEVEL 2

#define US_TICKER_TIMER HG_TIMER0_DEVID
#define UART_IOT_TIMER  HG_TIMER2_DEVID

#define MEM_DMA_ENABLE

#ifndef LED_INIT_BLINK
#define LED_INIT_BLINK 1
#endif

#ifndef DSLEEP_PAPWRCTL_DIS
#define DSLEEP_PAPWRCTL_DIS 0
#endif

#ifndef DC_DC_1_3V
#define DC_DC_1_3V 0
#endif

#ifndef MCU_FUNCTION
#define MCU_FUNCTION 0
#else
#define MCU_PIR_SENSOR_MUX_SPI_IO
#endif

#ifndef WNB_NEW_KEY
#define WNB_NEW_KEY 0
#endif

#ifndef AUTO_SLEEP_TIME
#define AUTO_SLEEP_TIME 10
#endif

struct sys_config {
    uint16_t magic_num, crc;
    uint16_t size, rev1, rev2, rev3;
    uint8_t  wnbcfg[512+40*WNB_STA_COUNT];
    uint32_t dsleep_test_cfg[4];
    uint32_t deep_sleep_time;
    uint8_t  atcmd_echo, atcmd_echo1, atcmd_echo2, atcmd_echo3;
};
extern struct sys_config sys_cfgs;

#endif
