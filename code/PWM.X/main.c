/*
 * File:   main.c
 * Author: cobrien
 *
 * Created on 09 February 2021, 19:46
 */


#include <xc.h>
#include <stdint.h>

#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit 
#pragma config CP = OFF         // Flash Program Memory Code Protection bit 

#define DUTY 50
#define _XTAL_FREQ 4000000
#define TMR2Prescale 1          // TMR2 Prescale value
uint16_t PWMFreq = 440;         // PWM Frequency

void PWM_Init(void)
{
    TRISBbits.TRISB6 = 1;                   // Set RB6 as input 
    RB6PPSbits.RB6PPS = 0b001101;           // set PWM 6 to output on RB6 
    PWM6CON = 0;                            // Clear PWM7CON register
    T2PRbits.PR = (_XTAL_FREQ/(PWMFreq*4*TMR2Prescale))-1;  // PWM period
    PWM6DC = (DUTY * 4 * (T2PRbits.PR + 1) / 100)<<6 ;       // Set duty to half of period
    PIR4bits.TMR2IF = 0;            // clear int flag 
    T2CLKCONbits.CS = 0b0001;       // Fosc/4
    T2CONbits.CKPS = 0;             // Prescaler of 0 
    T2CONbits.T2ON = 1;             // Turn on timer 
        
    TRISBbits.TRISB6 = 0;           // set RB6 as output 
    PWM6CONbits.EN = 1;             // enable PWM 
    return;
    
}

void main(void) {

    PWM_Init(); 
    while(1){
    }
    
}
