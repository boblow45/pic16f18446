/*
 * File:   main.c
 * Author: cillian
 *
 * Created on 21 February 2021, 19:48
 */


#include <xc.h>
#include <stdint.h>

#include "./inc/usart.h"

#define _XTAL_FREQ 4000000

#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit 
#pragma config CP = OFF         // Flash Program Memory Code Protection bit 


void __interrupt() INTERRUPT_InterruptManager (void)
{
    // interrupt handler
    if(INTCONbits.PEIE == 1)
    {
        if(PIE3bits.TX1IE == 1 && PIR3bits.TX1IF == 1)
        {
            usart_tx_irq();
        } 
        else if(PIE3bits.RC1IE == 1 && PIR3bits.RC1IF == 1)
        {
            usart_rx_irq();
        } 
        else
        {
            //Unhandled Interrupt
        }
    }      
    else
    {
        //Unhandled Interrupt
    }
}




void main(void) {
    
    char read_val;
    char output[] = "Hello World!\r\n";
    usart_init();
    usart_write(output, sizeof(output)/sizeof(output[0]) - 1);
    while(1){
        
        read_val = usart_read();
        usart_write(&read_val, 1);
        //__delay_ms(1000);
    }
}
