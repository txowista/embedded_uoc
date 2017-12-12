/*
 * accelerometer_driver.h
 *
 *  Created on: 26 oct. 2017
 *      Author: toni
 */

#ifndef ACCELEROMETER_DRIVER_H_
#define ACCELEROMETER_DRIVER_H_

#define CONVERSION_SCALE    3250.0
#define CONVERSION_OFFSET   8150.0
#define REFERENCE_TEMP 25.0f
enum axisType{
    x,
    y,
    z
};

void init_Accel(void);

void Accel_read(float *values);

void Accel_reverse(char *str, int len);

int Accel_intToStr(int x, char str[], int d);

void Accel_ftoa(float n, char *res, int afterpoint);

float Accel_correctTemp(float temp2modified, enum axisType currentAxis);

#endif /* ACCELEROMETER_DRIVER_H_ */
