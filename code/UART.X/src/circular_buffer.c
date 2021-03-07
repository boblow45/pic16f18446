#include <stdint.h>
#include <xc.h> 
#include "./../inc/circular_buffer.h"

void cbuf_init(struct c_buff_s * cbuf, uint8_t size){
    cbuf->head = 0;
    cbuf->tail = 0;
    cbuf->max = size;
    cbuf->full = false;
}


void cbuf_put(struct c_buff_s * cbuf, char * buff, char data){
    buff[cbuf->head] = data;
    cbuf->head = (cbuf->head + 1) % cbuf->max;

    if(cbuf->full){
        cbuf->tail = (cbuf->tail + 1) % cbuf->max;
    }
    cbuf->full = (cbuf->head == cbuf->tail);
}

char cbuf_get(struct c_buff_s * cbuf, char * buff){

    if(!(!cbuf->full && (cbuf->head == cbuf->tail))){
        char data = buff[cbuf->tail];
        cbuf->full = false;
        cbuf->tail = (cbuf->tail + 1)  % cbuf->max;
        return data;
    }

    return 0;

}

bool cbuf_empty(struct c_buff_s * buff) {
	return (!buff->full && (buff->head == buff->tail));
}

bool cbuf_full(struct c_buff_s * buff){
    return buff->full;
}