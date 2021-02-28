#ifndef CIR_BUFF_H
#define	CIR_BUFF_H

#include <stdint.h>
#include <stdbool.h>

struct c_buff_s{
    uint8_t head;
    uint8_t tail;
    uint8_t max;
    bool full;

};

void cbuf_init(struct c_buff_s * cbuf, uint8_t size);
void cbuf_put(struct c_buff_s * cbuf, char * buff, char data);
char cbuf_get(struct c_buff_s * cbuf, char * buff);
bool cbuf_empty(struct c_buff_s * buff);
bool cbuf_full(struct c_buff_s * buff);

#endif
