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

uint32_t beat_ms = 100;
volatile uint16_t TMR_val =0;
volatile uint8_t count = 0;

uint16_t freq[] = {A4, 0, A4,0, A4,0, F4,0, C5,0, A4,0, F4,0, C5,0, A4,0};

uint16_t cal_pwm_reload_reg(uint32_t delay_ms); 
void set_period(uint16_t freq);
void config_pwm(uint16_t freq); 
void timer_setup(void);

void __interrupt() isr(){   
    if(PIR0bits.TMR0IF){

        config_pwm(freq[count]);
        count++;
        count %= sizeof(freq)/sizeof(freq[0]);
        
        TMR_val = cal_pwm_reload_reg(beat_ms); 
        TMR0H = (TMR_val >> 8) & 0xFF;
        TMR0L = TMR_val & 0xFF;   
        PIR0bits.TMR0IF = 0;
    }
}

uint16_t cal_pwm_reload_reg(uint32_t delay_ms){
    T0CON1bits.T0CKPS = 4;
    uint32_t reg_val;  
    reg_val = (delay_ms * (_XTAL_FREQ / 4 / (1 << T0CON1bits.T0CKPS)) / 1000);
    
    while(reg_val > 0xFFFF){
        T0CON1bits.T0CKPS += 1;
        reg_val = (delay_ms * (_XTAL_FREQ / 4 / (1 << T0CON1bits.T0CKPS)) / 1000);
    }
    return 0xFFFF - reg_val;
}

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

void config_pwm(uint16_t freq){
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

void timer_setup(void){
    INTCONbits.GIE = 1;         // Enable the interrupt system
    INTCONbits.PEIE = 1;        // Enable interrupts from peripherals
    
    T0CON0bits.T016BIT = 1;     // 16 bit mode 
    T0CON1bits.T0CS = 2;        // Fosc / 4
    T0CON1bits.T0ASYNC = 0;     // TMR0 counter is synchronized to FOSC/4
    T0CON1bits.T0CKPS = 2;      // prescaler of 1
    T0CON0bits.T0OUTPS = 0;     // Postscaler of 2
    TMR0 = 0xFFFF;
    T0CON0bits.T0EN = 1; 
    PIE0bits.TMR0IE = 1;
}


void main(void) {

    timer_setup(); 
    


    while(1){
    }
    
}
