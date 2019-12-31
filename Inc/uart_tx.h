#ifndef __UART_TX_H__
#define __UART_TX_H__

#include <stddef.h>

typedef struct uart_tx_queue_pkg_s  {
    uint8_t msg_id;
        #define UART_TX_MSG0_SEND_SYNC  (0)
        #define UART_TX_MSG1_SEND_DATA  (1)
        #define UART_TX_MSG2_DMA_DONE   (2)
    uint8_t len; // the length of payload *data
    uint8_t dummy[2];
    uint8_t *payload; // a pointer returned by pvPortMalloc()
} uart_tx_queue_pkg_t;


#define UART_TX_BUF_SZ (256)
typedef struct {
    uint8_t buf[UART_TX_BUF_SZ];
    uint8_t dma_on_going;
    uint16_t wptr;
    uint16_t rptr;
} uart_tx_t;

#endif // __UART_TX_H__
