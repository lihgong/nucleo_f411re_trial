#include <string.h>
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"


extern UART_HandleTypeDef huart2;
extern osMessageQueueId_t uart_tx_queueHandle;


#if 0

static QueueHandle_t uart_tx_queue;

#define UART_TX_QUEUE_DEPTH     (4)

#define UART_TX_BUF_SZ (1024)
typedef struct {
    uint8_t buf[UART_TX_BUF_SZ];
    uint8_t dma_on_going;
    uint16_t wptr;
    uint16_t rptr;
} uart_tx;

#endif

typedef struct uart_tx_queue_pkg_s  {
    uint8_t msg_id;
        #define UART_TX_MSG1_SEND_SYNC  (0)
        #define UART_TX_MSG0_SEND_DATA  (1)
    uint8_t dummy[3];

    void *data;
} uart_tx_queue_pkg_t;



//uart_tx_queue_pkg_t *pkg;
void task_uart_tx_entry(void *argument)
{
    /* USER CODE BEGIN task_uart_tx_entry */

    /* Infinite loop */
    for(;;) {
        uart_tx_queue_pkg_t *pkg;
        osMessageQueueGet(uart_tx_queueHandle, &pkg, NULL, osWaitForever);

        uint32_t msg_id = pkg->msg_id;
        if(msg_id == UART_TX_MSG1_SEND_SYNC) {
            char str_to_send[] = "SYNC_STR\r\n";
            HAL_UART_Transmit(&huart2, (uint8_t*)str_to_send, sizeof(str_to_send), HAL_MAX_DELAY);
        } else if(msg_id == UART_TX_MSG0_SEND_DATA) {
            char str_to_send[] = "SEND_DATA_MSG\r\n";
            HAL_UART_Transmit(&huart2, (uint8_t*)str_to_send, sizeof(str_to_send), HAL_MAX_DELAY);
        } else {
            char str_to_send[] = "UNSUPPORTED MSG\r\n";
            HAL_UART_Transmit(&huart2, (uint8_t*)str_to_send, sizeof(str_to_send), HAL_MAX_DELAY);
        }

        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

        vPortFree(pkg);
    }
    /* USER CODE END task_uart_tx_entry */
}

