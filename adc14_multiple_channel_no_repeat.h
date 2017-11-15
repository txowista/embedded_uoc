/*
 * adc14_multiple_channel_no_repeat.h
 *
 *  Created on: 26 oct. 2017
 *      Author: toni
 */
// Includes FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#ifndef ADC14_MULTIPLE_CHANNEL_NO_REPEAT_H_
#define ADC14_MULTIPLE_CHANNEL_NO_REPEAT_H_

#define NUM_ADC_CHANNELS 3

SemaphoreHandle_t *xBinarySemaphoreADC;

#include <stdint.h>

volatile uint8_t ADC_reading_available;

typedef struct
{
    float x;
    float y;
    float z;
} axis;

axis readAxis;
extern void convertBuffer(void);
extern void init_ADC(SemaphoreHandle_t *xBinarySemaphore); //uses interrupt ISR ADC14_IRQHandler

extern axis *ADC_read(void);



#endif /* ADC14_MULTIPLE_CHANNEL_NO_REPEAT_H_ */
