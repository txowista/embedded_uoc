// Includes standard
#ifndef UTIL_H_
#define UTIL_H_
#include <stdio.h>
#include <stdint.h>
#include "math.h"
void utilPrintf_(uint32_t moduleInstance, char *message);
void utilUinttochar(char* a, uint32_t n);
void utilReverse(char *str, int len);
int utilIntToStr(int x, char str[], int d);
void utilFtoa(float n, char *res, int afterpoint);
#endif
