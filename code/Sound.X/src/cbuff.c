/*
 * File:   cbuff.c
 * Author: cillian
 *
 * Created on 07 March 2021, 18:58
 */

#include <stdbool.h>
#include <stdint.h>

#include "./../inc/cbuff.h"

void buff_init(struct cbuff_s * cbuf, char * buff, uint16_t size){
    cbuf->buff = buff;
    cbuf->head = 0;
    cbuf->tail = 0;
    cbuf->size = size;
    cbuf->full = false;
}

bool buf_empty(struct cbuff_s * cbuf) {
	return (!cbuf->full && (cbuf->head == cbuf->tail))? true: false;
}

void buf_put(struct cbuff_s * cbuf, char data){
    cbuf->buff[cbuf->head] = data;
    cbuf->head = (cbuf->head + 1) % cbuf->size;

    if(cbuf->full){
        cbuf->tail = (cbuf->tail + 1) % cbuf->size;
    }
    cbuf->full = (cbuf->head == cbuf->tail)? true: false;
}

char buf_get(struct cbuff_s * cbuf){
    char data;
    data = cbuf->buff[cbuf->tail];
    cbuf->full = false;
    cbuf->tail = (cbuf->tail + 1) % cbuf->size;
    return data;
}

char buf_view(struct cbuff_s * cbuf, uint16_t offset){
    uint16_t i = cbuf->tail + offset;
    return cbuf->buff[i % cbuf->size];
}

void buf_clear(struct cbuff_s * cbuf){
    cbuf->head = 0;
    cbuf->tail = 0;
    cbuf->full = false;
}

void buf_mv_tail_fwd(struct cbuff_s * cbuf, uint16_t offset){
    uint16_t i = cbuf->tail + offset;
    cbuf->tail = i % cbuf->size; 
    cbuf->full = false;
}
