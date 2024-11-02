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
#include "hal/gpio.h"
#include "hal/watchdog.h"
#include "hal/sysaes.h"
#include "hal/uart.h"
#include "hal/timer_device.h"
#include "hal/dma.h"
#include "lib/common.h"
#include "lib/syscfg.h"
#include "lib/skb/skb.h"
#include "lib/skb/skb_list.h"
#include "lib/mac_bus.h"
#include "lib/lmac/lmac.h"
#include "lib/lmac/lmac_host.h"
#include "lib/wnb/libwnb.h"
#include "lib/atcmd/libatcmd.h"

#ifdef IOT_DEV_UART
#define UARTIOT_RBSIZE (2048)
struct uart_iot {
    struct os_semaphore sema;
    RBUFFER_IDEF(rb, uint8, UARTIOT_RBSIZE);
    struct os_task task;
    struct dma_device *dma;
    struct uart_device *uart;
    struct timer_device *timer;
    uint8 txbuf[1500];
} uartiot;

void uart_receive(uint8 *data, int32 len);

static void uart_iot_task(void *args)//接收数据
{
    int32 count = 0;
    while (1) {
        count = 0;
        os_sema_down(&uartiot.sema, osWaitForever);
        while (count < 1500 && !RB_EMPTY(&uartiot.rb)) {
            RB_GET(&uartiot.rb, uartiot.txbuf[count]);
            count++;
        }
        if (count > 0) {
            uart_receive(uartiot.txbuf, count);
        }
    }
}

static void uart_iot_timer_cb(uint32 args)
{
    os_sema_up(&uartiot.sema);
}

static int32 uart_iot_irq_hdl(uint32 irq_flag, uint32 param1, uint32 param2)
{
    int32 ret = 0;
    switch (irq_flag) {
        case UART_INTR_RX_DATA_AVAIL:
            timer_device_stop(uartiot.timer);
            if (RB_COUNT(&uartiot.rb) > (UARTIOT_RBSIZE - 32)) {
                os_sema_up(&uartiot.sema);
            }
            RB_SET(&uartiot.rb, (uint8)param2);
            timer_device_start(uartiot.timer, UART_IOT_TIMER_MS*1000, uart_iot_timer_cb, (uint32)&uartiot);
            break;
        default:
            break;
    }
    return ret;
}

__init void uart_iot_init(void)
{
    RB_INIT(&uartiot.rb, UARTIOT_RBSIZE);
    os_sema_init(&uartiot.sema, 0);
    uartiot.timer = (struct timer_device *)dev_get(UART_IOT_TIMER);
    ASSERT(uartiot.timer);
    timer_device_open(uartiot.timer, TIMER_TYPE_ONCE, 0);
    uartiot.uart = (struct uart_device *)dev_get(HG_UART0_DEVID);
    ASSERT(uartiot.uart);
    uartiot.dma = (struct dma_device *)dev_get(HG_DMAC_DEVID);
    ASSERT(uartiot.dma);
    uart_ioctl(uartiot.uart, UART_IOCTL_CMD_USE_DMA, (uint32)uartiot.dma);
    uart_open(uartiot.uart);
    uart_baudrate(uartiot.uart, UART_IOT_BAUDRATE);
    uart_parity(uartiot.uart, UART_IOT_PARITY);
    uart_databits(uartiot.uart, UART_IOT_DATABITS);
    uart_stopbits(uartiot.uart, UART_IOT_STOPBITS);
    uart_request_irq(uartiot.uart, uart_iot_irq_hdl, 0x81, 0);
    OS_TASK_INIT("uart_iot", &uartiot.task, uart_iot_task, 0, OS_TASK_PRIORITY_NORMAL, 512);
}

void uart_send(uint8 *data, uint32 len)
{
    ASSERT(uartiot.uart);
    uart_puts(uartiot.uart, data, len);
}

#endif

