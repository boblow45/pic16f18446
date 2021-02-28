#include <xc.h> 
#include <stdbool.h>

#include "./../inc/usart.h"

#define TX_BUFF_SIZE (12)
#define RX_BUFF_SIZE (64)

char tx_buff[TX_BUFF_SIZE];
char rx_buff[RX_BUFF_SIZE];

struct cbuff_s{
    char * buff;
    uint8_t head;
    uint8_t tail;
    uint8_t size;
    bool full;
};

struct cbuff_s cbuff_tx;
struct cbuff_s cbuff_rx;

static void buff_init(struct cbuff_s * cbuf, char * buff, uint8_t size){
    cbuf->buff = buff;
    cbuf->head = 0;
    cbuf->tail = 0;
    cbuf->size = size;
    cbuf->full = false;
}

static bool buf_empty(struct cbuff_s * cbuf) {
	return (!cbuf->full && (cbuf->head == cbuf->tail))? true: false;
}

static void buf_put(struct cbuff_s * cbuf, char data){
    cbuf->buff[cbuf->head] = data;
    cbuf->head = (cbuf->head + 1) % cbuf->size;

    if(cbuf->full){
        cbuf->tail = (cbuf->tail + 1) % cbuf->size;
    }
    cbuf->full = (cbuf->head == cbuf->tail)? true: false;
}

static char buf_get(struct cbuff_s * cbuf){
    char data;
    data = cbuf->buff[cbuf->tail];
    cbuf->full = false;
    cbuf->tail = (cbuf->tail + 1) % cbuf->size;
    return data;
}

void usart_init(void){
    // disable interrupts before changing states
    PIE3bits.RC1IE = 0;
    PIE3bits.TX1IE = 0;
    
    buff_init(&cbuff_tx, tx_buff, TX_BUFF_SIZE);
    buff_init(&cbuff_rx, rx_buff, RX_BUFF_SIZE);

    TRISB = (1 << 5);   // Set RB5 as input
    
    RX1PPSbits.PORT = 0b01;     // Port B
    RX1PPSbits.PIN  = 5;        // Pin 5
    ANSELBbits.ANSB5 = 0;
    
    RB4PPS = 0b001111;          // Set Port B4 to TX1     

    // ABDOVF no_overflow; SCKP Non-Inverted; BRG16 16bit_generator; WUE disabled; ABDEN disabled; 
    BAUD1CON = 0x08;

    // SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled; 
    RC1STA = 0x90;

    // TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave; 
    TX1STA = 0x24;

    // SP1BRGL 51; 
    SP1BRGL = 51;

    // SP1BRGH 0; 
    SP1BRGH = 0x00;
  
    INTCONbits.GIE = 1;         // Enable the interrupt system
    INTCONbits.PEIE = 1;        // Enable interrupts from peripherals
    PIE3bits.RC1IE = 1;
}

void usart_write(char *data, uint8_t len){
    
    for(uint8_t i=0; i<len; i++){
        while(cbuff_tx.full){}
        buf_put(&cbuff_tx, data[i]);
        
        if (!PIE3bits.TX1IE){
            PIE3bits.TX1IE = 1;
        }
    }
}
char usart_read(void){
    while(buf_empty(&cbuff_rx)){}
    return buf_get(&cbuff_rx);
}

void usart_tx_irq(void){
    TX1REG = buf_get(&cbuff_tx);
    if(buf_empty(&cbuff_tx)){
        PIE3bits.TX1IE = 0;
    }
}

void usart_rx_irq(void){
    char data = RC1REG;
    buf_put(&cbuff_rx, data);
}
