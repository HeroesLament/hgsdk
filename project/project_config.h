#ifndef __SYS_PROJECT_CONFIG_H__
#define __SYS_PROJECT_CONFIG_H__
#define SYSCFG_ENABLE
#define OTA_ENABLE

#define AUTO_ETHERNET_PHY
//#define ETHERNET_IP101G
#define ETHERNET_PHY_ADDR               -1

#define DEFAULT_SYS_CLK    96000000UL //options: 32M/48M/72M/144M, and 16*N from 64M to 128M

#define ATCMD_UARTDEV      HG_UART1_DEVID

#define DEEP_SLEEP

#define GMAC_ENABLE

#define WNB_STA_COUNT      (8)        //donot change this count

//#define MACBUS_GMAC
#define MACBUS_NDEV
//#define MACBUS_COMBI

//#define LWIP_GMAC
//#define DHCPD_ENABLE

#define MACBUS_COMBI_LOCAL   MAC_BUS_TYPE_NDEV
#define MACBUS_COMBI_FORWARD MAC_BUS_TYPE_EMAC

#define MEM_TRACE
#define MEM_OFCHK

//#define TIMER1_ENABLE_MS      1  //range:1~85
//#define TIMER3_ENABLE_MS      50 //range:1~85

//#define IOT_DEV_UART
//#define UART_IOT_BAUDRATE     115200
//#define UART_IOT_PARITY       UART_PARIRY_NONE
//#define UART_IOT_DATABITS     UART_DATA_BIT_8
//#define UART_IOT_STOPBITS     UART_STOP_BIT_1
//#define UART_IOT_TIMER_MS     50

//#define IOT_DEV_SPI
//#define SPI_IOT_CLK          1000000
//#define SPI_IOT_CLK_MODE     SPI_CPOL_0_CPHA_0



#endif
