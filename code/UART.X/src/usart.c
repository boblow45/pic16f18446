#include <xc.h> 
#include <stdbool.h>

#include "./../inc/usart.h"

volatile uint8_t uart_bytes = 0;
volatile char * uart_data;

#define TX_BUFF_SIZE (16)
#define RX_BUFF_SIZE (8)

char tx_buff[TX_BUFF_SIZE];
volatile uint8_t tx_head;
volatile uint8_t tx_tail;
volatile bool tx_full;

static void tx_buff_init(void){
    tx_head = 0;
    tx_tail = 0;
    tx_full = false;
}

static bool tx_buf_empty() {
	return (!tx_full && (tx_head == tx_tail));
}

static void tx_buf_put(char data){
    tx_buff[tx_head] = data;
    tx_head = (tx_head + 1) % TX_BUFF_SIZE;

    if(tx_full){
        tx_tail = (tx_tail + 1) % TX_BUFF_SIZE;
    }
    tx_full = (tx_head == tx_tail)? true: false;
}

static char tx_buf_get(void){
    char data;
    data = tx_buff[tx_head];
    tx_full = false;
    tx_tail = (tx_tail + 1) % TX_BUFF_SIZE;
    return data;
}


void usart_write(char *data, uint8_t len){
    
    for(uint8_t i=0; i<len; i++){
        while(tx_full){}
        tx_buf_put(data[i]);
        PIE3bits.TX1IE = 1;
    }
}

void usart_init(void){
    // disable interrupts before changing states
    PIE3bits.RC1IE = 0;
    PIE3bits.TX1IE = 0;
    
    tx_buff_init();
    
    TRISBbits.TRISB4 = 1;       // Set RB4 as output
    RX1PPSbits.PORT = 0b01;     // Port B
    RX1PPSbits.PIN  = 5;        // Pin 5
            
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
    //PIE3bits.RC1IE = 1;
}

void usart_tx_irq(void){
    TX1REG = tx_buf_get();
    if(tx_buf_empty()){
        PIE3bits.TX1IE = 0;
    }
}
