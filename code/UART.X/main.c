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
            // Need to add rx code here 
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
    
    char output[] = "Hello World!\r\n";
    usart_init();
    while(1){
        usart_write(output, sizeof(output)/sizeof(output[0]) - 1);
        
        // __delay_ms(100);
    }
}
