// Includes standard
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Includes FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
// Includes drivers
#include "uart_driver.h"
#include "timer_A1.h"
#include "i2c_driver.h"
#include "temp_sensor.h"
#include "opt3001.h"
#include "tmp006.h"
// Variables del sensor de temperatura interno
float temperature;
char temp_string[10];
char light_string[10];
// Variables del sensor OPT3001
uint16_t rawData;
float convertedLux;
//Buffer Circular
typedef struct {
    float cbValue[10];
    int postion;
} circularBuffer;
circularBuffer cbTemperature={
    {0.0f},0
};
circularBuffer cbLight={
    {0.0f},0
};
// Declaracion de un semaforo binario
SemaphoreHandle_t xBinarySemaphore;
// Declaracion de un mutex
SemaphoreHandle_t xMutex;
// Prototipos de funciones privadas
static void prvSetupHardware(void);

// Definicion de prioridades de tareas
#define prvLED_TASK_PRIORITY    1
#define prvREAD_TEMP_TASK_PRIORITY    2
#define prvREAD_LIGHT_TASK_PRIORITY    2
#define prvWRITE_SERIAL_TASK_PRIORITY    4

static void prvRedLedTask(void *pvParameters);
static void prvReadTempTask(void *pvParameters);
static void prvReadLightTask(void *pvParameters);
static void prvWriteSerialTask(void *pvParameters);
static void writeCircularBuffer(circularBuffer *cb,float value);
void OnUart_rxChar( void );
static void writeSerial();

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
//    CS_setDCOFrequency(8000000);

    // Inicializa los clocks HSMCLK, SMCLK, MCLK y ACLK
    MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_PJ,
            GPIO_PIN0 | GPIO_PIN1,
            GPIO_PRIMARY_MODULE_FUNCTION);

    // Configuracion de frecuencias de clocks externos
    // LFXT = 32 kHz, HFXT = 48 MHz
    CS_setExternalClockSourceFrequency(32000, 48000000);

    // Selecciona el nivel de tension del core
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE0);

    // Inicializacion del I2C
    I2C_init();

    // Inicializacion de la UART
    UartInit(OnUart_rxChar);

    // Inicializacion del sensor TMP006
    TMP006_init();

    // Inicializacion del sensor opt3001
    sensorOpt3001Init();
    sensorOpt3001Enable(true);

    // Configura el pin P1.0 como salida (LED)
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);

    // Configuracion del pin P1.1 como entrada con R de pull-up
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);

    // Reset del flag de interrupcion del pin P1.1
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1);
    // Habilita la interrupcion del pin P1.1
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1);
    // Configura la prioridad de la interrupcion del PORT1
    MAP_Interrupt_setPriority(INT_PORT1, 0xA0);
    // Habilita la interrupcion del PORT1
    MAP_Interrupt_enableInterrupt(INT_PORT1);
    // Habilita que el procesador responda a las interrupciones
    MAP_Interrupt_enableMaster();

    // Habilita todas las interruciones
    __enable_interrupt();
}

/**
 * main.c
 */
void main(void)
{
    // Inicializacion del mutex
    xMutex = xSemaphoreCreateMutex();
    // Inicializacion del semaforo binario
    xBinarySemaphore = xSemaphoreCreateBinary();

    if ((xBinarySemaphore != NULL) && (xMutex != NULL))
    {
        // Inicializacion del hardware (clocks, GPIOs, IRQs)
        prvSetupHardware();
        // Creacion de tarea RedLedTask
        xTaskCreate(prvRedLedTask, // Puntero a la funcion que implementa la tarea
                "RedLedTask",              // Nombre descriptivo de la tarea
                configMINIMAL_STACK_SIZE,  // Tama�o del stack de la tarea
                NULL,                      // Argumentos de la tarea
                prvLED_TASK_PRIORITY,  // Prioridad de la tarea
                NULL);
        xTaskCreate(prvReadTempTask, // Puntero a la funcion que implementa la tarea
                "ReadTempTask",              // Nombre descriptivo de la tarea
                configMINIMAL_STACK_SIZE,  // Tama�o del stack de la tarea
                NULL,                      // Argumentos de la tarea
                prvREAD_TEMP_TASK_PRIORITY,  // Prioridad de la tarea
                NULL);
        xTaskCreate(prvReadLightTask, // Puntero a la funcion que implementa la tarea
                "ReadLightTask",              // Nombre descriptivo de la tarea
                configMINIMAL_STACK_SIZE,  // Tama�o del stack de la tarea
                NULL,                      // Argumentos de la tarea
                prvREAD_LIGHT_TASK_PRIORITY,  // Prioridad de la tarea
                NULL);
        xTaskCreate(prvWriteSerialTask, // Puntero a la funcion que implementa la tarea
                "WriteSerialTask",              // Nombre descriptivo de la tarea
                configMINIMAL_STACK_SIZE,  // Tama�o del stack de la tarea
                NULL,                      // Argumentos de la tarea
                prvREAD_LIGHT_TASK_PRIORITY,  // Prioridad de la tarea
                NULL);
        // Puesta en marcha de las tareas creadas
        vTaskStartScheduler();
    }
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
// Tarea RedLedTask
static void prvReadTempTask(void *pvParameters)
{
    static const TickType_t xReadTemp = pdMS_TO_TICKS(1000);
    // La tarea se repite en un bucle infinito
    for (;;)
    {
        // Intenta coger el mutex, bloqueandose si no esta disponible
        xSemaphoreTake(xMutex, portMAX_DELAY);
        {
            // Lee el valor de la medida de temperatura
            temperature = TMP006_readAmbientTemperature();
            if(temperature!=0)writeCircularBuffer(&cbTemperature,temperature);
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(xReadTemp);
    }

}
// Tarea RedLightTask
static void prvReadLightTask(void *pvParameters)
{
    static const TickType_t xReadLight = pdMS_TO_TICKS(1000);
    // La tarea se repite en un bucle infinito
    for (;;)
    {
        // Intenta coger el mutex, bloqueandose si no esta disponible
        xSemaphoreTake(xMutex, portMAX_DELAY);
        {
            // Lee el valor de la medida de temperatura
            sensorOpt3001Read(&rawData);
            sensorOpt3001Convert(rawData, &convertedLux);
            if(convertedLux!=0)writeCircularBuffer(&cbLight,convertedLux);
            xSemaphoreGive(xMutex);
        }
        vTaskDelay(xReadLight);

    }

}
static void prvWriteSerialTask(void *pvParameters)
{
    // Tiempo maximo de espera entre dos interrupciones del pulsador
    const TickType_t xMaxExpectedBlockTime = pdMS_TO_TICKS( 500 );

    // La tarea se repite en un bucle infinito
    for(;;) {
        // El semaforo debe ser entregado por la ISR PORT1_IRQHandler
        // Espera un numero maximo de xMaxExpectedBlockTime ticks
        if( xSemaphoreTake( xBinarySemaphore, xMaxExpectedBlockTime ) == pdPASS )
        {
            // Intenta coger el mutex, bloqueandose si no esta disponible
            xSemaphoreTake( xMutex, portMAX_DELAY );
            {
                writeSerial();
            }
            // Libera el mutex
            xSemaphoreGive( xMutex );
        }
    }

}
static void writeSerial(){
    int reciente = 0;
    reciente = cbTemperature.postion > 0 ? cbTemperature.postion - 1 : 9;
    // Convierte el valor de medida en cadena de caracteres
    ftoa(cbTemperature.cbValue[reciente], temp_string, 2);
    // Envia el valor de la medida a la UART
    UartPrint("Temperature = ");
    strcat(temp_string, " \n\r");
    UartPrint(temp_string);

    reciente = cbLight.postion > 0 ? cbLight.postion - 1 : 9;
    ftoa(cbLight.cbValue[reciente], light_string, 2);
    UartPrint("Light = ");
    strcat(light_string, " \n\r");
    UartPrint(light_string);

}
static void writeCircularBuffer(circularBuffer *cb,float value){
        cb->cbValue[cb->postion] = value;
        cb->postion = cb->postion < 9 ? cb->postion+1 : 0;

}
// Rutina de Servicio a Interrupcion (ISR) del PORT1
void PORT1_IRQHandler(void)
{
    uint32_t status;
    static BaseType_t xHigherPriorityTaskWoken;

    // Lee el estado de la interrupcion generada por GPIO_PORT_P1
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    // Reset del flag de interrupcion del pin que la genera
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    // Chequea si la interrupcion la genero el pin P1.1
    if(status & GPIO_PIN1)
    {
        // El parametro xHigherPriorityTaskWoken debe inicializarse
        // en pdFALSE, ya que se establecera en pdTRUE dentro de la
        // funcion API de interrupcion segura si se requiere un cambio
        // de contexto
        xHigherPriorityTaskWoken = pdFALSE;
        // Entrega el semaforo para desbloquear la tarea SenderTask
        xSemaphoreGiveFromISR(xBinarySemaphore,
                &xHigherPriorityTaskWoken);

        // Pasa xHigherPriorityTaskWoken en portYIELD_FROM_ISR().
        // Si xHigherPriorityTaskWoken valia pdTRUE dentro de
        // xSemaphoreGiveFromISR(), entonces al llamar a
        // portYIELD_FROM_ISR() solicitara un cambio de contexto.
        // Si xHigherPriorityTaskWoken sigue siendo pdFALSE, la llamada
        // a portYIELD_FROM_ISR() no tendra ningun efecto
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    }
}
// Funcion call-back ejecutada al recibir caracter en UART
void OnUart_rxChar( void )
{
    char data;
    // Lee el caracter recibido en la UART
    UartGetChar(&data);
}
