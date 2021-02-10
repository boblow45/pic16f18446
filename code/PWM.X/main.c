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

#define C0 16
#define Cs0 17
#define D0 18
#define Ds0 19
#define E0 21
#define F0 22
#define Fs0 23
#define G0 25
#define Gs0 26
#define A0 28
#define As0 29
#define B0 31

#define C4 262
#define Cs4 277
#define D4 294
#define Ds4 311
#define E4 230
#define F4 349
#define Fs4 370
#define G4 392
#define Gs4 415
#define A4 440
#define As4 466
#define B4 494
#define C5 523

#define DUTY 50
#define _XTAL_FREQ 4000000

void set_period(uint16_t freq){
    uint16_t pwm_period = _XTAL_FREQ/(freq*4);
    uint8_t prescaler = 0;
    while (pwm_period > 255){
        pwm_period /=2;
        prescaler++;
    }
    
    T2PRbits.PR = (uint8_t)pwm_period;
    T2CONbits.CKPS = prescaler;
    PWM6DC = (DUTY * 4 * (pwm_period + 1) / 100)<<6; 
    
    return;
}

void config_pwm(uint16_t freq)
{
    TRISBbits.TRISB6 = 1;                   // Set RB6 as input 
    RB6PPSbits.RB6PPS = 0b001101;           // set PWM 6 to output on RB6 
    PWM6CON = 0;                            // Clear PWM7CON register
    
    set_period(freq);
    PIR4bits.TMR2IF = 0;            // clear int flag 
    T2CLKCONbits.CS = 0b0001;       // Fosc/4
    T2CONbits.T2ON = 1;             // Turn on timer 
        
    TRISBbits.TRISB6 = 0;           // set RB6 as output 
    PWM6CONbits.EN = 1;             // enable PWM 
    return;
    
}

void main(void) {

//    uint16_t freq[] = {261, 277, 293, 311, 329, 349, 369, 392, 415, 440, 466, 493, 534};
    uint16_t freq[] = {A4, 0, A4,0, A4,0, F4,0, C5,0, A4,0, F4,0, C5,0, A4,0};
    uint8_t count = 0;
    while(1){
        if (freq[count]){
            config_pwm(freq[count] * 7);
            __delay_ms(500);
        }
        else{
            TRISBbits.TRISB6 = 1;
            __delay_ms(10);
        }
        
        
        count++;
        count %= sizeof(freq)/sizeof(freq[0]);
    }
    
}
