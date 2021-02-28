#include <xc.h> 

#include "./../inc/usart.h"

volatile uint8_t uart_bytes = 0;
volatile char * uart_data;


void usart_write(char *data, uint8_t len){
    while(uart_bytes){}
    uart_bytes = len -1;
    uart_data = data;
    PIE3bits.TX1IE = 1;
}

void usart_init(void){
    // disable interrupts before changing states
    PIE3bits.RC1IE = 0;
    PIE3bits.TX1IE = 0;
    
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
    TX1REG = *uart_data++;
    if((--uart_bytes) == 0){
        PIE3bits.TX1IE = 0;
    }
}
