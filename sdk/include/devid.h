#ifndef _HUGEIC_DEV_ID_H_
#define _HUGEIC_DEV_ID_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HG_UART0_DEVID = 1,
    HG_UART1_DEVID,
    HG_USB11_DEV_DEVID,
    HG_HWAES_DEVID,
    HG_SPI0_DEVID,
    HG_SPI1_DEVID,
    HG_SPI2_DEVID,
    HG_SPI3_DEVID,
    HG_DMAC_DEVID,
    HG_LMAC_DEVID,
    HG_CIPHER_DEVID,
    HG_AHPHY_DEVID,
    HG_TDMA_DEVID,
    HG_TDMA2_DEVID,
    HG_I2C0_DEVID,
    HG_EFUSE_DEVID,
    HG_GPIOA_DEVID,
    HG_GPIOB_DEVID,
    HG_WDT32K_DEVID,
    HG_WDTV1_DEVID,
    HG_SDIOSLAVE_DEVID,
    HG_TIMER0_DEVID,
    HG_TIMER1_DEVID,
    HG_TIMER2_DEVID,
    HG_TIMER3_DEVID,
    HG_DBGPATH_DEVID,
    HG_RFSPI_DEVID,
    HG_RFDIGICALI_DEVID,
    HG_S1GRF_DEVID,
    HG_MIPI_DEVID,
    HG_GMAC_DEVID,
    HG_M2MDMA_DEVID,
    HG_SPI_FLASH0_DEVID,
    HG_CRC_DEVID,
    HG_ETHPHY0_DEVID,
    HG_ETH_MDIOBUS0_DEVID,
    HG_WIFI_DEVID,
} DEV_ID;

#ifdef __cplusplus
}
#endif

#endif
