/*
 * File:   main.c
 * Author: cobrien
 *
 * Created on 08 February 2021, 23:23
 */


#include <xc.h>
#include <stdint.h>

#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit 
#pragma config CP = OFF         // Flash Program Memory Code Protection bit 

#define _XTAL_FREQ 4000000

static void initSystem(void){
    
}

static void initADC(void){
    // POT1 is on RC0
    
    ADCON0bits.FM = 1;                  //right justify
    ADCON0bits.CS = 1;                  //FRC Clock
    ADPCHbits.ADPCH = 0b010000;         //RC0 is Analog channel
    TRISCbits.TRISC0 = 1;               //Set RC0 to input
    ANSELCbits.ANSC0 = 1;               //Set RC0 to analog 
    ADCON0bits.ON = 1;                  //Turn ADC On
}

uint16_t result;
void main(void) {
    
    initSystem();
    initADC();
    
    while(1){
        ADCON0bits.GO = 1;              //Start conversion
        while (ADCON0bits.GO){}         //Wait for conversion done
        
        result = ADRESH << 8 | ADRESL;            //Read result
    }
    return;
}
