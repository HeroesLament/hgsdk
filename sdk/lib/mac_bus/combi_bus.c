
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
#include "lib/lmac/hgic.h"

#ifdef MACBUS_COMBI

#define FRULE_COUNT (64)
#define FRULE_TMO   (1000)

struct combi_bus_frule {
    uint32 lifetime;
    uint8  addr[6];
};

struct mac_bus_combi {
    struct mac_bus              bus;
    struct mac_bus             *local;
    struct mac_bus             *forward;
    struct combi_bus_frule      frules[FRULE_COUNT];
};

extern struct wnb_config wnbcfg;

static void combi_bus_frule_expire(struct mac_bus_combi *combi_bus)
{
    int32 i = 0;
    for (i = 0; i < FRULE_COUNT; i++) {
        if (combi_bus->frules[i].lifetime && TIME_AFTER(os_jiffies(), combi_bus->frules[i].lifetime + os_msecs_to_jiffies(FRULE_TMO))) {
            combi_bus->frules[i].lifetime = 0;
        }
    }
}
static void combi_bus_frule_update(struct mac_bus_combi *combi_bus, uint8 *addr)
{
    int32 i = 0;
    int32 f = -1;
    for (i = 0; i < FRULE_COUNT; i++) {
        if (combi_bus->frules[i].lifetime && os_memcmp(combi_bus->frules[i].addr, addr, 6) == 0) {
            combi_bus->frules[i].lifetime = os_jiffies();
            return;
        } else if (combi_bus->frules[i].lifetime == 0 && f == -1) {
            f = i;
        }
    }

    if (f != -1) {
        os_memcpy(combi_bus->frules[f].addr, addr, 6);
        combi_bus->frules[f].lifetime = os_jiffies();
    }
}
static int32 combi_bus_frule_forward(struct mac_bus_combi *combi_bus, uint8 *addr)
{
    int32 i = 0;
    for (i = 0; i < FRULE_COUNT; i++) {
        if (combi_bus->frules[i].lifetime && os_memcmp(combi_bus->frules[i].addr, addr, 6) == 0) {
            return 1;
        }
    }
    return 0;
}

static int32 combi_bus_write(struct mac_bus *bus, uint8 *buff, int32 len)
{
    uint8  local  = 0;
    uint8  hdrlen = 0;
    uint8 *dest = buff;
    struct mac_bus_combi *combi_bus = container_of(bus, struct mac_bus_combi, bus);

    //os_printf("WIFI: "MACSTR"<->"MACSTR", %x\r\n", MAC2STR(dest), MAC2STR(dest+6), get_unaligned_be16(dest+12) );
    local = (os_memcmp(wnbcfg.addr, dest, 6) == 0);
    if (local || (dest[0] & 0x1)) {
        //os_printf("    --> to local\r\n");
        combi_bus->local->write(combi_bus->local, dest, len - hdrlen);
    }
    if (!local || (dest[0] & 0x1)) {
        //os_printf("    --> to gmac\r\n");
        combi_bus->forward->write(combi_bus->forward, buff, len);
    }
    combi_bus->bus.txerr = combi_bus->local->txerr + combi_bus->forward->txerr;
    return RET_OK;
}

static void combi_bus_recv(struct mac_bus *bus, struct sk_buff *skb, int len)
{
    uint8  hdrlen = 0;
    uint8 *dest   = skb->data;
    uint16 proto  = get_unaligned_be16(skb->data + 12);
    struct mac_bus_combi *combi_bus = (struct mac_bus_combi *)bus->priv;

    //os_printf("%s: "MACSTR"<->"MACSTR", %x\r\n", bus == combi_bus->local?"LOCAL":"GMAC",
    //    MAC2STR(dest), MAC2STR(dest+6), get_unaligned_be16(dest+12) );
    if (bus == combi_bus->local && ((dest[0] & 0x1) || combi_bus_frule_forward(combi_bus, dest))) {
        //os_printf("    --> to gmac\r\n");
        combi_bus->forward->write(combi_bus->forward, dest, len - hdrlen);
        if (!(dest[0] & 0x1)) {
            kfree_skb(skb);
            return;
        }
    } else if (bus == combi_bus->forward && ((dest[0] & 0x1) || os_memcmp(wnbcfg.addr, dest, 6) == 0)) {
        //os_printf("    --> to local\r\n");
        combi_bus->local->write(combi_bus->local, dest, len - hdrlen);
        combi_bus_frule_update(combi_bus, dest + 6);
        if (!(dest[0] & 0x1) && proto != 0x4847) {
            kfree_skb(skb);
            return;
        }
    }
    //os_printf("    --> to wifi\r\n");
    combi_bus->bus.recv(&combi_bus->bus, skb, len);
    combi_bus->bus.rxerr = combi_bus->local->rxerr + combi_bus->forward->rxerr;
}

__init struct mac_bus *mac_bus_combi_attach(mac_bus_recv recv, void *priv)
{
    struct mac_bus_combi *combi_bus = os_zalloc(sizeof(struct mac_bus_combi));
    ASSERT(combi_bus && recv);
    combi_bus->bus.write = combi_bus_write;
    combi_bus->bus.type  = MAC_BUS_TYPE_NDEV;
    combi_bus->bus.recv  = recv;
    combi_bus->bus.priv  = priv;
    combi_bus->local   = mac_bus_attach(MACBUS_COMBI_LOCAL, combi_bus_recv, combi_bus);
    ASSERT(combi_bus->local);
    combi_bus->forward = mac_bus_attach(MACBUS_COMBI_FORWARD, combi_bus_recv, combi_bus);
    ASSERT(combi_bus->forward);
    return &combi_bus->bus;
}

#endif

