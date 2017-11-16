/*
 * adc14_multiple_channel_no_repeat.h
 *
 *  Created on: 26 oct. 2017
 *      Author: toni
 */
#ifndef ADC14_MULTIPLE_CHANNEL_NO_REPEAT_H_
#define ADC14_MULTIPLE_CHANNEL_NO_REPEAT_H_
// Includes FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdint.h>


#define NUM_ADC_CHANNELS 3
#define POINT_FOR_G 2730
#define ZERO_G 3
SemaphoreHandle_t *xBinarySemaphoreADC;


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
