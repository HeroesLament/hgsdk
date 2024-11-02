
#include "typesdef.h"
#include "list.h"
#include "dev.h"
#include "devid.h"
#include "osal/string.h"
#include "osal/mutex.h"
#include "osal/semaphore.h"
#include "osal/task.h"
#include "hal/sdio_slave.h"
#include "hal/gpio.h"
#include "lib/skb/skb.h"
#include "lib/mac_bus.h"
#include "lib/lmac/hgic.h"

struct mac_bus_sdio {
    struct mac_bus     bus;
    struct os_mutex    tx_lock;
    struct sdio_slave *dev;
    struct sk_buff    *rx_skb;
    int32              ready;
    int32              part_len;
    uint8              rx_buff[2048];
};

static int32 sdio_bus_write(struct mac_bus *bus, uint8 *buff, int32 len)
{
    int32 ret = RET_OK;
    struct mac_bus_sdio *sdio_bus = container_of(bus, struct mac_bus_sdio, bus);

    if (!sdio_bus->ready) {
        sdio_bus->bus.txerr++;
        return RET_ERR;
    }
    ret = sdio_slave_write(sdio_bus->dev, buff, ALIGN(len, 4), TRUE);
    if (ret) {
        sdio_bus->bus.txerr++;
        sdio_bus->ready = 0;
    }
    return ret;
}

static void sdio_bus_irq(uint32 irq, uint32 param1, uint32 param2, uint32 param3)
{
    struct mac_bus_sdio *sdio_bus = (struct mac_bus_sdio *)param1;
    struct hgic_hdr *hdr = (struct hgic_hdr *)sdio_bus->rx_buff;
    //gpio_set_val(1, 1);
    switch (irq) {
        case SDIO_SLAVE_IRQ_RX:
#ifdef SDIO_PART_TRANS
            if (sdio_bus->rx_skb) {
                struct hgic_frm_hdr2 *hdr = (struct hgic_frm_hdr2 *)sdio_bus->rx_skb->data;
                if (sdio_bus->part_len == 0 && hdr->hdr.magic == HGIC_HDR_TX_MAGIC && param3 < hdr->hdr.length) {
                    sdio_bus->part_len = param3;
                    break;
                }
                if (sdio_bus->part_len > 0) param3 += sdio_bus->part_len;
            }
            sdio_bus->part_len = 0;
#endif
            if (sdio_bus->rx_skb) {
                sdio_bus->bus.recv(&sdio_bus->bus, sdio_bus->rx_skb, param3);
            }else if(hdr->magic == HGIC_HDR_TX_MAGIC &&
                    (hdr->type == HGIC_HDR_TYPE_CMD || hdr->type == HGIC_HDR_TYPE_CMD2)){ // only recv CMD/CMD2
                struct sk_buff *nskb  = alloc_tx_skb(1900);
                if(nskb == NULL) nskb = alloc_skb(param3+4);
                if(nskb){
                    os_memcpy(nskb->data, sdio_bus->rx_buff, param3);
                    sdio_bus->bus.recv(&sdio_bus->bus, nskb, param3);
                }
                os_memset(sdio_bus->rx_buff, 0, 8);
            }

            sdio_bus->rx_skb = alloc_tx_skb(1900);
            if (sdio_bus->rx_skb) {
                skb_reserve(sdio_bus->rx_skb, 256);
                sdio_slave_read(sdio_bus->dev, sdio_bus->rx_skb->data, DATA_AREA_SIZE, FALSE);
            } else {
                sdio_bus->bus.rxerr++;
                sdio_slave_read(sdio_bus->dev, sdio_bus->rx_buff, 2048, FALSE);
            }
            sdio_bus->ready = 1;
            break;
        case SDIO_SLAVE_IRQ_RESET:
            if (!sdio_bus->rx_skb) {
                sdio_bus->rx_skb = alloc_tx_skb(1900);
                if(sdio_bus->rx_skb) skb_reserve(sdio_bus->rx_skb, 256);
            }
            if (sdio_bus->rx_skb) {
                sdio_slave_read(sdio_bus->dev, sdio_bus->rx_skb->data, DATA_AREA_SIZE, FALSE);
            } else {
                sdio_slave_read(sdio_bus->dev, sdio_bus->rx_buff, 2048, FALSE);
            }
            break;
        default:
            break;
    }
    //gpio_set_val(1, 0);
}

struct mac_bus *mac_bus_sdio_attach(mac_bus_recv recv, void *priv)
{
    int ret = RET_OK;
    struct mac_bus_sdio *sdio_bus = os_zalloc(sizeof(struct mac_bus_sdio));
    ASSERT(sdio_bus && recv);
    os_mutex_init(&sdio_bus->tx_lock);
    sdio_bus->bus.write = sdio_bus_write;
    sdio_bus->bus.type  = MAC_BUS_TYPE_SDIO;
    sdio_bus->bus.recv  = recv;
    sdio_bus->bus.priv  = priv;
    sdio_bus->dev = (struct sdio_slave *)dev_get(HG_SDIOSLAVE_DEVID);
    ASSERT(sdio_bus->dev);
    ret = sdio_slave_open(sdio_bus->dev, SDIO_SLAVE_SPEED_48M, 0); //SDIO_SLAVE_SPEED_4M
    ASSERT(!ret);
    os_printf("sdio open, ret=%d\n", ret);
    sdio_bus->rx_skb = alloc_tx_skb(1900);
    ASSERT(sdio_bus->rx_skb);
    skb_reserve(sdio_bus->rx_skb, 256);
    sdio_slave_read(sdio_bus->dev, sdio_bus->rx_skb->data, DATA_AREA_SIZE, FALSE);
    ASSERT(!ret);
    sdio_slave_request_irq(sdio_bus->dev, sdio_bus_irq, (uint32)sdio_bus);
    ASSERT(!ret);
    return &sdio_bus->bus;
}

__init void mac_bus_sdio_detach(struct mac_bus *bus)
{
    ASSERT(bus);
    struct mac_bus_sdio *sdio_bus = container_of(bus, struct mac_bus_sdio, bus);
    sdio_slave_close(sdio_bus->dev);
    if(sdio_bus->rx_skb) {
        kfree_skb(sdio_bus->rx_skb);
        sdio_bus->rx_skb = NULL;
    }
    os_mutex_del(&sdio_bus->tx_lock);
    os_free(sdio_bus);
}


