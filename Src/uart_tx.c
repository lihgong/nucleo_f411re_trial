#include <string.h>
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "uart_tx.h"

extern UART_HandleTypeDef huart2;
extern osMessageQueueId_t uart_tx_queueHandle;


uart_tx_t g_uart_tx;



//uart_tx_queue_pkg_t *pkg;
void task_uart_tx_entry(void *argument)
{
    /* USER CODE BEGIN task_uart_tx_entry */

    /* Infinite loop */
    for(;;) {
        uart_tx_queue_pkg_t *pkg;
        osMessageQueueGet(uart_tx_queueHandle, &pkg, NULL, osWaitForever);

        uint32_t msg_id = pkg->msg_id;
        uint32_t len = pkg->len;
        uint8_t *payload = UART_TX_PAYLOAD_P(pkg);

        if(msg_id == UART_TX_MSG0_SEND_SYNC) {
            char str_to_send[] = "SYNC_STR\r\n";
            HAL_UART_Transmit(&huart2, (uint8_t*)str_to_send, sizeof(str_to_send), HAL_MAX_DELAY);
        } else if(msg_id == UART_TX_MSG1_SEND_DATA) {
            char str_to_send[] = "SEND_DATA_MSG\r\n";
            HAL_UART_Transmit(&huart2, (uint8_t*)str_to_send, sizeof(str_to_send), HAL_MAX_DELAY);
            HAL_UART_Transmit(&huart2, payload, len, HAL_MAX_DELAY);
        } else {
            char str_to_send[] = "UNSUPPORTED MSG\r\n";
            HAL_UART_Transmit(&huart2, (uint8_t*)str_to_send, sizeof(str_to_send), HAL_MAX_DELAY);
        }

        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

        vPortFree(pkg);
    }
    /* USER CODE END task_uart_tx_entry */
}

