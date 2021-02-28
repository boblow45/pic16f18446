#ifndef USART_H
#define	USART_H

#include <stdint.h>

void usart_init(void);
void usart_write(char *data, uint8_t len);
char usart_read(void);
void usart_tx_irq(void);
void usart_rx_irq(void);

#endif	/* USART_H*/

