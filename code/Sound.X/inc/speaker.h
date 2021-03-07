
#ifndef SPEAKER_H
#define	SPEAKER_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>


void timer_setup(void);
void timer_irq(void);
void turn_on_speaker(void);
void turn_off_speaker(void);
void update_tempo(uint32_t val);
void update_note(uint16_t note, uint16_t loc);
void update_num_notes(uint8_t notes);
void update_duration(uint16_t dur, uint16_t loc);

#endif	/* SPEAKER_H */

