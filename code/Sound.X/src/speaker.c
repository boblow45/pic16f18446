#include <xc.h>
#include <stdbool.h>

#include "./../inc/speaker.h"
#include "./../inc/defines.h"


#define DUTY 50
#define NUM_NOTES (128)


uint32_t beat_ms = 250;
uint8_t off_note_ms = 1;
volatile uint16_t TMR_val =0;
volatile uint8_t count = 0;
volatile uint8_t playing = 0;
uint8_t num_notes = 0; 

uint16_t freq[NUM_NOTES];
uint16_t duration[NUM_NOTES];

static uint16_t cal_pwm_reload_reg(uint32_t delay_ms); 
static void set_period(uint16_t freq);
static void config_pwm(uint16_t freq); 

void update_note(uint16_t note, uint16_t loc){
    if (loc < NUM_NOTES){
        freq[loc] = note;
    }
    else{}
}

void update_duration(uint16_t dur, uint16_t loc){
    if (loc < NUM_NOTES){
        duration[loc] = dur;
    }
    else{}
}

void update_num_notes(uint8_t notes){
    num_notes =notes;
}

void timer_irq(void){
    
    if (!playing){
        
        TRISBbits.TRISB6 = 1;
        TMR_val = cal_pwm_reload_reg(duration[count] - off_note_ms); 
        TMR0H = (TMR_val >> 8) & 0xFF;
        TMR0L = TMR_val & 0xFF;   

        if (freq[count]){
            config_pwm(freq[count]);
            TRISBbits.TRISB6 = 0;
        }
        else{
            TRISBbits.TRISB6 = 1;
        }
        
        playing = 1;
        count++;
        count %= num_notes;
    }
    else{
        playing = 0;
        TMR_val = cal_pwm_reload_reg(off_note_ms); 
        TMR0H = (TMR_val >> 8) & 0xFF;
        TMR0L = TMR_val & 0xFF;   
        TRISBbits.TRISB6 = 1;
    }
    PIR0bits.TMR0IF = 0;
}


void timer_setup(void){
    INTCONbits.GIE = 1;         // Enable the interrupt system
    INTCONbits.PEIE = 1;        // Enable interrupts from peripherals
    
    T0CON0bits.T016BIT = 1;     // 16 bit mode 
    T0CON1bits.T0CS = 2;        // Fosc / 4
    T0CON1bits.T0ASYNC = 0;     // TMR0 counter is synchronized to FOSC/4
    T0CON1bits.T0CKPS = 2;      // prescaler of 1
    T0CON0bits.T0OUTPS = 0;     // Postscaler of 2
    TMR0H = 0xFF;
    TMR0L = 0XFF;
    T0CON0bits.T0EN = 0; 
    PIE0bits.TMR0IE = 0;

}

void update_tempo(uint32_t val){
    beat_ms = val;
}

void turn_on_speaker(void){
    count = 0;
    playing = false;
    T0CON0bits.T0EN = 1; 
    PIE0bits.TMR0IE = 1;
    TRISBbits.TRISB6 = 0;
}

void turn_off_speaker(void){
    T0CON0bits.T0EN = 0; 
    PIE0bits.TMR0IE = 0;
    TRISBbits.TRISB6 = 1;
}


static uint16_t cal_pwm_reload_reg(uint32_t delay_ms){
    T0CON1bits.T0CKPS = 4;
    uint32_t reg_val;  
    reg_val = (delay_ms * (_XTAL_FREQ / 4 / (1 << T0CON1bits.T0CKPS)) / 1000);
    
    while(reg_val > 0xFFFF){
        T0CON1bits.T0CKPS += 1;
        reg_val = (delay_ms * (_XTAL_FREQ / 4 / (1 << T0CON1bits.T0CKPS)) / 1000);
    }
    return 0xFFFF - reg_val;
}

static void set_period(uint16_t freq){
    uint16_t pwm_period = _XTAL_FREQ/(freq*4);
    uint8_t prescaler = 0;
    while (pwm_period > 255){
        pwm_period /=2;
        prescaler++;
    }
    
    T2PRbits.PR = (uint8_t)pwm_period;
    T2CONbits.CKPS = prescaler;
    uint16_t duty_cycle = (DUTY * 4 * (pwm_period + 1) / 100)<<6; 
    PWM6DCL = (uint8_t)(duty_cycle & 0xFF);
    PWM6DCH = (uint8_t)((duty_cycle >> 8) & 0xFF);
    return;
}

static void config_pwm(uint16_t freq){

    RB6PPSbits.RB6PPS = 0b001101;           // set PWM 6 to output on RB6 
    PWM6CON = 0;                            // Clear PWM7CON register
    
    set_period(freq);
    PIR4bits.TMR2IF = 0;            // clear int flag 
    T2CLKCONbits.CS = 0b0001;       // Fosc/4
    T2CONbits.T2ON = 1;             // Turn on timer 
        
    PWM6CONbits.EN = 1;             // enable PWM 
    return;
    
}