/*
 * acelerometer_driver.c
 *
 *  Created on: 26 oct. 2017
 *      Author: toni
 */

// Includes standard
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "accelerometer_driver.h"
#include "adc14_multiple_channel_no_repeat.h"

/* Reverses a string 'str' of length 'len' */
void Accel_reverse(char *str, int len)
{
    int i=0, j=len-1, temp;
    while (i<j)

    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++; j--;
    }
}

/* Converts a given integer x to string str[].  d is the number
  of digits required in output. If d is more than the number
  of digits in x, then 0s are added at the beginning */
int Accel_intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x%10) + '0';
        x = x/10;
    }

    /* If number of digits required is more, then add 0s at the beginning */
    while (i < d)
        str[i++] = '0';

    Accel_reverse(str, i);
    str[i] = '\0';
    return i;
}

/* Converts a floating point number to string. */
void Accel_ftoa(float n, char *res, int afterpoint)
{
    int next_pos = 0;
    int length;
    /* Check sign */
    if (n<0){
        res[0]='-';
        n = -n;
        next_pos=1;
    }

    /* Extract integer part */
    int ipart = (int)n;

    /* Extract floating part */
    float fpart = n - (float)ipart;

    /* convert integer part to string */
    length = Accel_intToStr(ipart, res+next_pos, 1);
    next_pos = next_pos + length;

    /* check for display option after point */
    if (afterpoint != 0)
    {
        res[next_pos] = '.';  /* add dot */
        next_pos++;

       /*  Get the value of fraction part upto given no.
         of points after dot. The third parameter is needed
         to handle cases like 233.007 */
        fpart = fpart * pow(10, afterpoint);
        Accel_intToStr((int)fpart, res + next_pos, afterpoint);
    }
}



void init_Accel(void){
    init_ADC();
}

void Accel_read(float *values){
    uint16_t *Data;
    uint8_t i;

    //pedir datos ADC
    Data = ADC_read();

    //realizar conversion
    for (i=0;i<NUM_ADC_CHANNELS;i++){
        values[i] = ((float)Data[i]-CONVERSION_OFFSET)/CONVERSION_SCALE;
    }

}


