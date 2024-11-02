
#include "typesdef.h"
#include "list.h"
#include "dev.h"
#include "devid.h"
#include "osal/string.h"
#include "osal/mutex.h"
#include "osal/task.h"
#include "osal/semaphore.h"
#include "osal/timer.h"
#include "hal/netdev.h"
#include "hal/dma.h"
#include "hal/netdev.h"
#include "lib/skb/skb.h"
#include "lib/mac_bus.h"
#include "lib/wnb/libwnb.h"
#include "lib/sysheap.h"

struct mac_bus_ndev {
    struct netdev               ndev;
    netdev_input_cb             input_cb;
    struct mac_bus              bus;
};

extern struct wnb_config wnbcfg;

static int32 ndev_bus_write(struct mac_bus *bus, uint8 *buff, int32 len)
{
    struct mac_bus_ndev *ndev_bus = container_of(bus, struct mac_bus_ndev, bus);
    if (ndev_bus && ndev_bus->input_cb) {
        ndev_bus->input_cb((struct netdev *)ndev_bus, buff, len);
    } else {
        ndev_bus->bus.txerr++;
    }
    return RET_OK;
}

static int32 ndev_bus_open(struct netdev *ndev, netdev_input_cb cb)
{
    struct mac_bus_ndev *ndev_bus = (struct mac_bus_ndev *)ndev;
    ndev_bus->input_cb = cb;
    wnb_open();
    return RET_OK;
}

static int32 ndev_bus_close(struct netdev *ndev)
{
    struct mac_bus_ndev *ndev_bus = (struct mac_bus_ndev *)ndev;
    ndev_bus->input_cb = NULL;
    wnb_close();
    return RET_OK;
}

static int32 ndev_bus_send_data(struct netdev *ndev, const void *p_data, uint32 size)
{
    struct sk_buff *skb = NULL;
    struct mac_bus_ndev *ndev_bus = (struct mac_bus_ndev *)ndev;

    if (ndev_bus->bus.recv) {
        skb = alloc_tx_skb(size + 256 + 32);
        if (skb) {
            skb_reserve(skb, 256);
            hw_memcpy(skb->data, p_data, size);
            ndev_bus->bus.recv(&ndev_bus->bus, skb, size);
        }else{
            os_printf("no more skb!\r\n");
        }
    }
    return 0;
}

static int32 ndev_bus_ioctl(struct netdev *ndev, uint32 cmd, uint32 param1, uint32 param2)
{
    int32 ret = RET_OK;
    //struct mac_bus_ndev *ndev_bus = (struct mac_bus_ndev *)ndev;

    switch (cmd) {
        case NETDEV_IOCTL_GET_ADDR:
            os_memcpy((uint8 *)param1, wnbcfg.addr, 6);
            break;
        case NETDEV_IOCTL_SET_ADDR:
            wnb_close();
            os_memcpy(wnbcfg.addr, (uint8 *)param1, 6);
            wnb_open();
            break;
        case NETDEV_IOCTL_GET_LINK_STATUS:
            break;
        case NETDEV_IOCTL_GET_LINK_SPEED:
            break;
        default:
            ret = -ENOTSUPP;
            break;
    }
    return ret;
}

__init struct mac_bus *mac_bus_ndev_attach(mac_bus_recv recv, void *priv)
{
    struct mac_bus_ndev *ndev_bus = os_zalloc(sizeof(struct mac_bus_ndev));
    ASSERT(ndev_bus && recv);
    ndev_bus->bus.write = ndev_bus_write;
    ndev_bus->bus.type  = MAC_BUS_TYPE_NDEV;
    ndev_bus->bus.recv  = recv;
    ndev_bus->bus.priv  = priv;

    ndev_bus->ndev.close = 0;
    ndev_bus->ndev.open              = ndev_bus_open;
    ndev_bus->ndev.close             = ndev_bus_close;
    ndev_bus->ndev.ioctl             = ndev_bus_ioctl;
    ndev_bus->ndev.send_data         = ndev_bus_send_data;

    dev_register(HG_WIFI_DEVID, (struct dev_obj *)ndev_bus);
    return &ndev_bus->bus;
}

