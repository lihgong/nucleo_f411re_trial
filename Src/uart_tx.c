#include <string.h>
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "uart_tx.h"

extern UART_HandleTypeDef huart2;
extern osMessageQueueId_t uart_tx_queueHandle;


uart_tx_t g_uart_tx;
#if 0
#define UART_TX_BUF_SZ (1024)
typedef struct {
    uint8_t buf[UART_TX_BUF_SZ];
    uint8_t dma_on_going;
    uint16_t wptr;
    uint16_t rptr;
} uart_tx_t;

#endif


void push_buf(uint8_t *payload, uint32_t len)
{
    uint8_t *buf = g_uart_tx.buf;
    uint32_t wptr = g_uart_tx.wptr;
    uint32_t remain_buf = UART_TX_BUF_SZ - wptr;
    if(remain_buf >= len) {
        memcpy(buf + wptr, payload, len);
        wptr += len;
        if(wptr >= UART_TX_BUF_SZ) {
            wptr = 0;
        }

    } else {
        memcpy(buf + wptr,  payload,             remain_buf);
        memcpy(buf,         payload+remain_buf,  len - remain_buf);
        wptr = len - remain_buf;
    }
    g_uart_tx.wptr = wptr;
}

void touch_dma(void)
{
    if(g_uart_tx.dma_on_going) {
        return;
    }

    uint32_t wptr = g_uart_tx.wptr;
    uint32_t rptr = g_uart_tx.rptr;
    if(wptr == rptr) {
        return; // nothing on the buffer
    }

    if(wptr > rptr) {
        HAL_UART_Transmit_DMA(&huart2, g_uart_tx.buf + rptr, wptr - rptr);
        g_uart_tx.rptr = wptr;
    } else { // wrap-around case
        HAL_UART_Transmit_DMA(&huart2, g_uart_tx.buf + rptr, UART_TX_BUF_SZ - rptr);
        g_uart_tx.rptr = 0;
        char *msg = "wrap around happening...\r\n";
        push_buf(msg, strlen(msg));
    }
    g_uart_tx.dma_on_going = 1;
}
//uart_tx_queue_pkg_t *pkg;
void task_uart_tx_entry(void *argument)
{
    /* USER CODE BEGIN task_uart_tx_entry */

    /* Infinite loop */
    for(;;) {
        uart_tx_queue_pkg_t pkg;
        osMessageQueueGet(uart_tx_queueHandle, &pkg, NULL, osWaitForever);

        uint32_t msg_id = pkg.msg_id;
        uint32_t len = pkg.len;
        uint8_t *payload = pkg.payload;

        if(msg_id == UART_TX_MSG0_SEND_SYNC) {
        } else if(msg_id == UART_TX_MSG1_SEND_DATA) {
            push_buf(payload, len);
            touch_dma();

        } else if(msg_id == UART_TX_MSG2_DMA_DONE) {
            g_uart_tx.dma_on_going = 0;
            touch_dma();


        } else {
            char str_to_send[] = "UNSUPPORTED MSG\r\n";
            //HAL_UART_Transmit(&huart2, (uint8_t*)str_to_send, sizeof(str_to_send), HAL_MAX_DELAY);
        }

        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

        if(payload != NULL) {
            vPortFree(payload);
        }
    }
    /* USER CODE END task_uart_tx_entry */
}


uint32_t ctr = 0;
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    ctr++;

    uart_tx_queue_pkg_t pkg = {
      .msg_id = UART_TX_MSG2_DMA_DONE,
      .len = 0,
      .payload = NULL,
    };
    osMessageQueuePut(uart_tx_queueHandle, &pkg, /*prio*/0, /*timeout*/0);
}


void uart_tx_send_str(char *str)
{
    uint32_t buf_sz = strlen(str) + 1;
    uart_tx_queue_pkg_t pkg = {
      .msg_id = UART_TX_MSG1_SEND_DATA,
      .len = buf_sz,
      .payload = pvPortMalloc(buf_sz),
    };
    strcpy(pkg.payload, str);

    // In this function, we would send the queue to receiver
    osMessageQueuePut(uart_tx_queueHandle, &pkg, /*prio*/0, osWaitForever);

}
