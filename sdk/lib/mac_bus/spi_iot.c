#include "typesdef.h"
#include "list.h"
#include "dev.h"
#include "devid.h"
#include "osal/string.h"
#include "osal/mutex.h"
#include "osal/task.h"
#include "osal/semaphore.h"
#include "osal/timer.h"
#include "hal/dma.h"
#include "hal/spi.h"
#include "hal/gpio.h"
#include "lib/ticker_api.h"
#include "lib/skb/skb.h"
#include "lib/mac_bus.h"
#include "lib/lmac/hgic.h"


#ifdef IOT_DEV_SPI

#define SPIBUS_RX_INT              PIN_SPI1_IO2

#define SPI_BUF_SIZE (2048)

struct spibus_iot {
    struct os_semaphore     sema;
    struct os_task          task;
    struct spi_device       *spi;
    uint8   buf[SPI_BUF_SIZE];
} spi_iot;

#define SPIBUS_CS_SEL       0

void spi_receive(uint8 *data);

void spibus_master_write(uint8 *buff, int32 len)
{
    spi_set_cs(spi_iot.spi, SPIBUS_CS_SEL, 0);
    spi_write(spi_iot.spi, buff, len);
    spi_set_cs(spi_iot.spi, SPIBUS_CS_SEL, 1);
}

static void spibus_master_rx_task(void *data)
{
    while (1) {
        os_sema_down(&spi_iot.sema, osWaitForever);
        spi_set_cs(spi_iot.spi, SPIBUS_CS_SEL, 0);
        spi_read(spi_iot.spi, spi_iot.buf, SPI_BUF_SIZE);
        spi_set_cs(spi_iot.spi, SPIBUS_CS_SEL, 1);
        spi_receive(spi_iot.buf);
    }
}

static void spibus_master_int_irq(int32 id, enum gpio_irq_event event)
{
    switch (event) {
        case GPIO_IRQ_EVENT_RISE:
            break;
        case GPIO_IRQ_EVENT_FALL:
            os_sema_up(&spi_iot.sema);
            //os_printf("rd\r\n");
            break;
        default:
            break;
    }
}

__init void spi_iot_master_init(void)
{
    os_sema_init(&spi_iot.sema, 0);
    spi_iot.spi = (struct spi_device *)dev_get(HG_SPI1_DEVID);
    ASSERT(spi_iot.spi);
    spi_open(spi_iot.spi, SPI_IOT_CLK, SPI_MASTER_MODE, SPI_WIRE_NORMAL_MODE, SPI_IOT_CLK_MODE);

    gpio_set_dir(SPIBUS_RX_INT, GPIO_DIR_INPUT);
    //gpio_set_mode(SPIBUS_RX_INT, GPIO_PULL_DOWN, GPIO_PULL_LEVEL_10K);
    gpio_enable_irq(SPIBUS_RX_INT, 1);
    gpio_request_pin_irq(SPIBUS_RX_INT, spibus_master_int_irq, 0, GPIO_IRQ_EVENT_FALL); // No consideration release irq now
    
    OS_TASK_INIT("spi_iot", &spi_iot.task, spibus_master_rx_task, 0, OS_TASK_PRIORITY_NORMAL, 512);
}

#endif


