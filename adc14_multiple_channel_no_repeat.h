/*
 * adc14_multiple_channel_no_repeat.h
 *
 *  Created on: 26 oct. 2017
 *      Author: toni
 */

#ifndef ADC14_MULTIPLE_CHANNEL_NO_REPEAT_H_
#define ADC14_MULTIPLE_CHANNEL_NO_REPEAT_H_

#define NUM_ADC_CHANNELS 3

#include <stdint.h>

volatile uint8_t ADC_reading_available;

typedef struct
{
    float x;
    float y;
    float z;
} axis;

axis readAxis;

extern void init_ADC(void); //uses interrupt ISR ADC14_IRQHandler

extern axis *ADC_read(void);




#endif /* ADC14_MULTIPLE_CHANNEL_NO_REPEAT_H_ */
