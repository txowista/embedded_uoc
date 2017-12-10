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
#define CONVERSION_SCALE    3250.0
#define CONVERSION_OFFSET   8150.0

#define ZERO_G 3
#define REFERENCE_TEMP 25.0f

enum axisType{
    x,
    y,
    z
};
typedef struct
{
    float x;
    float y;
    float z;
} axis;

axis readAxis;
extern void convertBuffer(void);
extern void init_ADC(); //uses interrupt ISR ADC14_IRQHandler
float correctTemp(float temp2modified, enum axisType currentAxis);
extern axis *ADC_read(void);



#endif /* ADC14_MULTIPLE_CHANNEL_NO_REPEAT_H_ */
