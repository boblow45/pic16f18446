/*
 * File:   main.c
 * Author: cobrien
 *
 * Created on 09 February 2021, 19:46
 */


#include <xc.h>
#include <stdint.h>

#include "./inc/usart.h"
#include "./inc/speaker.h"
#include "./inc/defines.h"

#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit 
#pragma config CP = OFF         // Flash Program Memory Code Protection bit 


void __interrupt() isr(){   
    
    if(INTCONbits.PEIE == 1){
        
        
        if(PIE3bits.TX1IE == 1 && PIR3bits.TX1IF == 1)
        {
            usart_tx_irq();
        } 
        else if(PIE3bits.RC1IE == 1 && PIR3bits.RC1IF == 1)
        {
            usart_rx_irq();
        } 
        else if(PIR0bits.TMR0IF){
            timer_irq();
        }
        else
        {
            //Unhandled Interrupt
        }
    }
}


void main(void) {
    TRISAbits.TRISA2 = 0; // set A2 as output
    LATAbits.LATA2 = 0;
    usart_init();
    timer_setup(); 
    turn_off_speaker();
    while(1){
        
        usart_process_cmd();
    }
    
}

