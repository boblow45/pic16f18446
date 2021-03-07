#ifndef CBUFF_H
#define	CBUFF_H

#include <stdbool.h>
#include <stdint.h>

struct cbuff_s{
    char * buff;
    uint16_t head;
    uint16_t tail;
    uint16_t size;
    bool full;
};

void buff_init(struct cbuff_s * cbuf, char * buff, uint16_t size);
bool buf_empty(struct cbuff_s * cbuf);
void buf_put(struct cbuff_s * cbuf, char data);
char buf_get(struct cbuff_s * cbuf);
char buf_view(struct cbuff_s * cbuf, uint16_t offset);
void buf_clear(struct cbuff_s * cbuf);
void buf_mv_tail_fwd(struct cbuff_s * cbuf, uint16_t offset);


#endif	/* CBUFF_H */

