// Includes standard
#include <stdio.h>
#include <stdint.h>
#include "math.h"
// Includes FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "adc14_multiple_channel_no_repeat.h"
#define prvLED_TASK_PRIORITY    1
#define READER_TASK_PRIORITY    2
#define UART_WRITER_TASK_PRIORITY    3
#define DELAY_100_MS     100
#define DELAY_500_MS     500
// UART Configuration Parameter (9600 bps - clock 8MHz)
const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        52,                                      // BRDIV = 78
        1,                                       // UCxBRF = 2
        0,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};
//funcion imprimir string por UART
void printf_(uint32_t moduleInstance, char *message){
    int index = 0;
    while(message[index]!='\0'){
        MAP_UART_transmitData(EUSCI_A0_BASE, message[index]);
        index++;
    }
}

//funcion pasar uint a char[x]
void uinttochar(char* a, uint32_t n)
{
  if (n == 0)
  {
    *a = '0';
    *(a+1) = '\0';
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
    n /=  10;
    c++;
  }

  memcpy(a, auxp, c);
}
/* Reverses a string 'str' of length 'len' */
void reverse(char *str, int len)
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
int intToStr(int x, char str[], int d)
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

    reverse(str, i);
    str[i] = '\0';
    return i;
}
/* Converts a floating point number to string. */
void ftoa(float n, char *res, int afterpoint)
{
    /* Extract integer part */
    int ipart = (int)n;

    /* Extract floating part */
    float fpart = n - (float)ipart;

    /* convert integer part to string */
    int i = intToStr(ipart, res, 0);

    /* check for display option after point */
    if (afterpoint != 0)
    {
        res[i] = '.';  /* add dot */

       /*  Get the value of fraction part upto given no.
         of points after dot. The third parameter is needed
         to handle cases like 233.007 */
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}
// Prototipos de funciones privadas
static void prvSetupHardware(void);
static void prvRedLedTask(void *pvParameters);
static void prvReaderTask(void *pvParameters);
static void prvUartWriterTask(void *pvParameters);
uint16_t *Data;
typedef struct {
    int x;
    int y;
    int z;
}axis;
xQueueHandle xQueue; // Variable para referenciar a la cola
static void prvSetupHardware(void)
{
    extern void FPU_enableModule(void);

    // Inicializacion de pins sobrantes para reducir consumo
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, PIN_ALL8);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_PB, PIN_ALL16);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_PC, PIN_ALL16);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_PD, PIN_ALL16);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_PE, PIN_ALL16);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, PIN_ALL8);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_PB, PIN_ALL16);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_PC, PIN_ALL16);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_PD, PIN_ALL16);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_PE, PIN_ALL16);

    // Habilita la FPU
    MAP_FPU_enableModule();
    // Cambia el numero de "wait states" del controlador de Flash
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);

    // Selecciona la frecuencia central de un rango de frecuencias del DCO
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    // Configura la frecuencia del DCO
    CS_setDCOFrequency(8000000);

    // Inicializa los clocks HSMCLK, SMCLK, MCLK y ACLK
    MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // Selecciona el nivel de tension del core
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE0);


    // Configura el pin P1.0 como salida (LED)
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    // Seleccion de modo UART en pines P1.2 y P1.3
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    // Configuracion de la UART
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);
    // Habilitacion de la UART
    MAP_UART_enableModule(EUSCI_A0_BASE);
    //Inicializa ADC
    init_ADC();

    // Habilita que el procesador responda a las interrupciones
    MAP_Interrupt_enableMaster();


}
/**
 * main.c
 */
void main(void)
{
    // Inicializacion del hardware (clocks, GPIOs, IRQs)
    prvSetupHardware();
    xQueue = xQueueCreate( 10, sizeof( axis ) );
    // Creacion de tarea RedLedTask
    xTaskCreate(prvRedLedTask, // Puntero a la funcion que implementa la tarea
            "RedLedTask",              // Nombre descriptivo de la tarea
            configMINIMAL_STACK_SIZE,  // Tama�o del stack de la tarea
            NULL,                      // Argumentos de la tarea
            prvLED_TASK_PRIORITY,  // Prioridad de la tarea
            NULL);
    // Creacion de tarea ReaderTask
    xTaskCreate( prvReaderTask,
                 "ReaderTask",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 READER_TASK_PRIORITY,
                 NULL );
    // Creacion de tarea ReaderTask
    xTaskCreate( prvUartWriterTask,
                 "WriterTask",
                 configMINIMAL_STACK_SIZE,
                 NULL,
                 UART_WRITER_TASK_PRIORITY,
                 NULL );
    // Puesta en marcha de las tareas creadas
    vTaskStartScheduler();
    return 0;
}
// Tarea RedLedTask
static void prvRedLedTask(void *pvParameters)
{
    // Calcula el tiempo de activacion del LED (en ticks)
    // a partir del tiempo en milisegundos
    static const TickType_t xBlinkOn = pdMS_TO_TICKS(10);

    // Calcula el tiempo de desactivacion del LED (en ticks)
    // a partir del tiempo en milisegundos
    static const TickType_t xBlinkOff = pdMS_TO_TICKS(990);

    // La tarea se repite en un bucle infinito
    for (;;)
    {
        // Fija a 1 la salida digital en pin 1.0 (LED on)
        MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
        // Bloquea la tarea durante el tiempo de on del LED
        vTaskDelay(xBlinkOn);
        // Fija a 0 la salida digital en pin 1.0 (LED off)
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
        // Bloquea la tarea durante el tiempo de off del LED
        vTaskDelay(xBlinkOff);
    }
}
// Tarea ReaderTask
static void prvReaderTask (void *pvParameters)
{
    for(;;){
        axis readAxis;
        vTaskDelay( pdMS_TO_TICKS(DELAY_100_MS) );
        Data = ADC_read();
        readAxis.x=Data[0];
        readAxis.y=Data[1];
        readAxis.z=Data[2];
        xQueueSendToBack( xQueue, &readAxis, 0 );
        vTaskDelay( pdMS_TO_TICKS(DELAY_100_MS) );
    }
}
// Tarea UARTWRITERTask
static void prvUartWriterTask (void *pvParameters)
{
    char num[20];
    const portTickType xTicksToWait = 100 / portTICK_RATE_MS;
    for(;;){
        axis writeAxis;
        xQueueReceive( xQueue, &writeAxis, xTicksToWait );
        printf_(EUSCI_A0_BASE, "\n\r");
        float xg=((float)writeAxis.x/2730)-3;
        float yg=((float)writeAxis.y/2730)-3;
        float zg=((float)writeAxis.z/2730)-3;
        ftoa(xg, num, 2);
        printf_(EUSCI_A0_BASE, "AccX: "); printf_(EUSCI_A0_BASE, num); printf_(EUSCI_A0_BASE, "\n\r");
        ftoa(yg, num, 2);
        printf_(EUSCI_A0_BASE, "AccY: "); printf_(EUSCI_A0_BASE, num); printf_(EUSCI_A0_BASE, "\n\r");
        ftoa(zg, num, 2);
        printf_(EUSCI_A0_BASE, "AccZ: "); printf_(EUSCI_A0_BASE, num); printf_(EUSCI_A0_BASE, "\n\r");
        vTaskDelay( pdMS_TO_TICKS(DELAY_500_MS) );
    }
}
