
#ifndef _MAC_BUS_H_
#define _MAC_BUS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define DATA_AREA_SIZE (1700)

enum mac_bus_type {
    MAC_BUS_TYPE_NONE,
    MAC_BUS_TYPE_SDIO,
    MAC_BUS_TYPE_USB,
    MAC_BUS_TYPE_EMAC,
    MAC_BUS_TYPE_UART,
    MAC_BUS_TYPE_NDEV,
    MAC_BUS_TYPE_QC,
    MAC_BUS_TYPE_COMBI,
};

enum MAC_BUS_EVENT {
    MAC_BUS_EVENT_RECVED,
};

struct mac_bus {
    int type;
    void *priv;
    uint32 rxerr, txerr;
    int (*write)(struct mac_bus *bus, unsigned char *buf, int len);
    void (*recv)(struct mac_bus *bus, struct sk_buff *skb, int len);
};

typedef void (*mac_bus_recv)(struct mac_bus *bus, struct sk_buff *skb, int len);
extern struct mac_bus *mac_bus_attach(int bus_type, mac_bus_recv recv, void *priv);
extern struct mac_bus *mac_bus_sdio_attach(mac_bus_recv recv, void *priv);
extern struct mac_bus *mac_bus_usb_attach(mac_bus_recv recv, void *priv);
extern struct mac_bus *mac_bus_uart_attach(mac_bus_recv recv, void *priv);
extern struct mac_bus *mac_bus_gmac_attach(mac_bus_recv recv, void *priv);
extern struct mac_bus *mac_bus_qa_gmac_attach(mac_bus_recv recv, void *priv);
extern struct mac_bus *mac_bus_qc_attach(mac_bus_recv recv, void *priv);
extern struct mac_bus *mac_bus_ndev_attach(mac_bus_recv recv, void *priv);
extern struct mac_bus *mac_bus_combi_attach(mac_bus_recv recv, void *priv);

extern void mac_bus_sdio_detach(struct mac_bus *bus);
extern void mac_bus_usb_detach(struct mac_bus *bus);
extern void mac_bus_gmac_detach(struct mac_bus *bus);

#ifdef __cplusplus
}
#endif

#endif

