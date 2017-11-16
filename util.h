// Includes standard
#ifndef UTIL_H_
#define UTIL_H_
#include <stdio.h>
#include <stdint.h>
#include "math.h"
void printf_(uint32_t moduleInstance, char *message);
void uinttochar(char* a, uint32_t n);
void reverse(char *str, int len);
int intToStr(int x, char str[], int d);
void ftoa(float n, char *res, int afterpoint);
#endif
