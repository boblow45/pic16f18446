/*
 * File:   newmain.c
 * Author: cobrien
 *
 * Created on 07 February 2021, 14:07
 */

#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit 
#pragma config CP = OFF         // Flash Program Memory Code Protection bit 

#define _XTAL_FREQ 4000000

#define LED2        PORTCbits.RC7
#define LED2_TRIS   TRISCbits.TRISC7

#include <xc.h>

void __interrupt() isr()
{   
    if(PIR0bits.INTF){
        PIR0bits.INTF = 0;
        if(LATAbits.LATA1 == 1){
            LATAbits.LATA1 = 0;
        }else{
            LATAbits.LATA1 = 1;
        }
    }
}

void init_portA(void){
    PORTA = 0x0;
    LATA = 0x0;
    ANSELA = 0x0;
    TRISAbits.TRISA0 =1;
    TRISAbits.TRISA1 =0;
    TRISAbits.TRISA2 =0;
    TRISAbits.TRISA5 =1;
}


void init_portC(void){
    PORTC = 0x0;
    LATC = 0x0;
    ANSELC = 0x0;
    TRISCbits.TRISC4 =1;
    TRISCbits.TRISC5 =0;
}


void main(void) {
    
    INTCONbits.GIE = 1;         // Enable the interrupt system
    INTCONbits.PEIE = 1;        // Enable interrupts from peripherals
    INTCONbits.INTEDG = 1;      // Int on rising edge
    
    INTPPSbits.PORT = 0;
    INTPPSbits.PIN = 0;
    
    PIE0bits.INTE = 1;
    
    init_portA();
    init_portC();
    
    char flag = 0;
    while(1){
        __delay_ms(1000);
        if(!PORTCbits.RC4){
            LATCbits.LATC5 = 1;
        }
        else{
            LATCbits.LATC5 = 0;
        }
        
        if (flag){
            flag = 0;
            LATAbits.LATA2 = 1;
        }
        else{
            flag = 1;
            LATAbits.LATA2 = 0;
        }
    }
}
