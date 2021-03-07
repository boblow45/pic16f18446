#include <xc.h> 
#include <stdbool.h>

#include "./../inc/usart.h"
#include "./../inc/speaker.h"
#include "./../inc/cbuff.h"

#define TX_BUFF_SIZE (16)
#define RX_BUFF_SIZE (256)

#define STX     (0x02)  // Start of packet flag
#define ETX     (0x03)  // End of packet flag
#define ESCAPE  (0x1B)  // Escape flag, so 0x02 and 0x03 can be used in the protocol

enum rx_state{await_str, received_str, escaping, processing};

enum usart_cmds{LED=0x13, 
                SOUND_OFF=0x14,
                SOUND_ON = 0x15,
                UPDATE_NOTES = 0x16,
                UPDATE_DURATION = 0x17,
                UPDATE_BEAT = 0x18, 
};


char tx_buff[TX_BUFF_SIZE];
char rx_buff[RX_BUFF_SIZE];

struct cbuff_s cbuff_tx;
struct cbuff_s cbuff_rx;

void usart_init(void){
    // disable interrupts before changing states
    PIE3bits.RC1IE = 0;
    PIE3bits.TX1IE = 0;
    
    buff_init(&cbuff_tx, tx_buff, TX_BUFF_SIZE);
    buff_init(&cbuff_rx, rx_buff, RX_BUFF_SIZE);

    TRISB = (1 << 5);   // Set RB5 as input
    
    RX1PPSbits.PORT = 0b01;     // Port B
    RX1PPSbits.PIN  = 5;        // Pin 5
    ANSELBbits.ANSB5 = 0;
    
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
    PIE3bits.RC1IE = 1;
}

void usart_write(char *data, uint8_t len){
    
    for(uint8_t i=0; i<len; i++){
        while(cbuff_tx.full){}
        buf_put(&cbuff_tx, data[i]);
        
        if (!PIE3bits.TX1IE){
            PIE3bits.TX1IE = 1;
        }
    }
}
char usart_read(void){
    while(buf_empty(&cbuff_rx)){}
    return buf_get(&cbuff_rx);
}

void usart_tx_irq(void){
    TX1REG = buf_get(&cbuff_tx);
    if(buf_empty(&cbuff_tx)){
        PIE3bits.TX1IE = 0;
    }
}



uint8_t state = await_str;

void change_state(char data){
    switch(data){
        case STX:
            state = received_str;
            break;
        case ESCAPE:
            state = escaping;
            break;
        case ETX:
            state = processing;
            break;
            
        default:
            break;
    }
}

void usart_rx_irq(void){
    char data = RC1REG;
    
    switch(state){
        case await_str:
            change_state(data);
            break;
        case received_str:
            change_state(data);
            if(state == received_str)
                buf_put(&cbuff_rx, data);
            break;
        case escaping:
            buf_put(&cbuff_rx, data);
            change_state(STX);
            break;      
        default:
            break;
            // Nothing. May need to send a none valid command 
    } 
}


static bool valid_cmd(void){
    return true;
}

uint32_t temp_32;
uint16_t temp_16;
uint8_t len;
void usart_process_cmd(void){
    
    
    if(!buf_empty(&cbuff_rx)){
        switch(buf_view(&cbuff_rx, 0)){
            case LED:
                LATAbits.LATA2 = (~LATAbits.LATA2) & 1;
                buf_mv_tail_fwd(&cbuff_rx, 4 + buf_view(&cbuff_rx, 1));
                state = await_str;
                break;
            case SOUND_ON:
                turn_on_speaker();
                buf_mv_tail_fwd(&cbuff_rx, 4 + buf_view(&cbuff_rx, 1));
                state = await_str;
                break;
            case SOUND_OFF:
                turn_off_speaker();
                buf_mv_tail_fwd(&cbuff_rx, 4 + buf_view(&cbuff_rx, 1));
                state = await_str;
                break;
            case UPDATE_BEAT:            
                temp_32 = 0;
                len = buf_view(&cbuff_rx, 1);
                for(uint16_t i=0; i<len;i++){
                    temp_32 |= ((uint32_t)buf_view(&cbuff_rx, 2 + i) << (8 * i));
                }
                update_tempo(temp_32);
                buf_mv_tail_fwd(&cbuff_rx, 4 + len);
                state = await_str;
                break;
                
            case UPDATE_NOTES:  
                                
                len = buf_view(&cbuff_rx, 1)/2;
                for(uint16_t i=0; i<len;i++){
                    temp_16 = 0; 
                    temp_16 |= (uint16_t)buf_view(&cbuff_rx, 2 + (i*2));
                    temp_16 |= (uint16_t)buf_view(&cbuff_rx, 2 + (i*2) + 1) << 8;
                    update_note(temp_16, i);
                }
                update_num_notes(len);
                buf_mv_tail_fwd(&cbuff_rx, 4 + buf_view(&cbuff_rx, 1));
                state = await_str;
                break;
                
            case UPDATE_DURATION:
                len = buf_view(&cbuff_rx, 1)/2;
                for(uint16_t i=0; i<len;i++){
                    temp_16 = 0; 
                    temp_16 |= (uint16_t)buf_view(&cbuff_rx, 2 + (i*2));
                    temp_16 |= (uint16_t)buf_view(&cbuff_rx, 2 + (i*2) + 1) << 8;
                    update_duration(temp_16, i);
                }
                update_num_notes(len);
                buf_mv_tail_fwd(&cbuff_rx, 4 + buf_view(&cbuff_rx, 1));
                state = await_str;
                break;
                
            default:
                break;
        
        }
    }
    
}