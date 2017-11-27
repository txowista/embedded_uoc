
// Includes standard
#include <stdio.h>
#include <stdint.h>
#include "math.h"



//funcion pasar uint a char[x]
void utilUinttochar(char* a, uint32_t n)
{
    if (n == 0)
    {
        *a = '0';
        *(a + 1) = '\0';
        return;
    }

    char aux[20];
    aux[19] = '\0';
    char* auxp = aux + 19;

    int c = 1;
    while (n != 0)
    {
        int mod = n % 10;
        *(--auxp) = mod | 0x30;
        n /= 10;
        c++;
    }

    memcpy(a, auxp, c);
}
/* Reverses a string 'str' of length 'len' */
void utilReverse(char *str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j)

    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}
/* Converts a given integer x to string str[].  d is the number
 of digits required in output. If d is more than the number
 of digits in x, then 0s are added at the beginning */
int utilIntToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    /* If number of digits required is more, then add 0s at the beginning */
    while (i < d)
        str[i++] = '0';

    utilReverse(str, i);
    str[i] = '\0';
    return i;
}
/* Converts a floating point number to string. */
void utilFtoa(float n, char *res, int afterpoint)
{
    /* Extract integer part */
    int negativo = 0;
    if (n < 0)
    {
        n *= -1;
        negativo = 1;
        res[0] = '-';
    }
    int ipart = (int) n;

    /* Extract floating part */
    float fpart = n - (float) ipart;

    /* convert integer part to string */
    int i = utilIntToStr(ipart, res + negativo, 0);
    if (i == 0)
    {
        res[negativo] = '0';
        i++;
    }
    i += negativo;
    /* check for display option after point */
    if (afterpoint != 0)
    {
        res[i] = '.'; /* add dot */

        /*  Get the value of fraction part upto given no.
         of points after dot. The third parameter is needed
         to handle cases like 233.007 */
        fpart = fpart * pow(10, afterpoint);

        utilIntToStr((int) fpart, res + i + 1, afterpoint);
    }
}
