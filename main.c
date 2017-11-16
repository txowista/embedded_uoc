// Includes standard
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "util.h"
// Includes FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "adc14_multiple_channel_no_repeat.h"
#define HEARTBEAT_TASK_PRIORITY    1
#define READER_TASK_PRIORITY    2
#define UART_WRITER_TASK_PRIORITY    1
#define DELAY_100_MS     100
#define DELAY_500_MS     500
#define QUEUE_SIZE  10
#define HEART_BEAT_ON_MS 10
#define HEART_BEAT_OFF_MS 990
// Declaracion de un semaforo binario
SemaphoreHandle_t xBinarySemaphore;
SemaphoreHandle_t xMutexUART;
// UART Configuration Parameter (9600 bps - clock 8MHz)
const eUSCI_UART_Config uartConfig = {
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
void printf_(uint32_t moduleInstance, char *message)
{
    int index = 0;
    while (message[index] != '\0')
    {
        MAP_UART_transmitData(EUSCI_A0_BASE, message[index]);
        index++;
    }
}
// Prototipos de funciones privadas
static void prvSetupHardware(void);
static void prvHeartBeatTask(void *pvParameters);
static void prvReaderTask(void *pvParameters);
static void prvUartWriterTask(void *pvParameters);
uint16_t *Data;
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
    MAP_CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_6);
    // Configura la frecuencia del DCO
    CS_setDCOFrequency(CS_8MHZ);

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
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    // Configuracion de la UART
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);
    // Habilitacion de la UART
    MAP_UART_enableModule(EUSCI_A0_BASE);
    //Inicializa ADC
    init_ADC(&xBinarySemaphore);
    // Habilita que el procesador responda a las interrupciones
    MAP_Interrupt_enableMaster();

}
/**
 * main.c
 */
void main(void)
{
    // Inicializacion del semaforo binario
    xBinarySemaphore = xSemaphoreCreateBinary();
    xMutexUART = xSemaphoreCreateMutex();
    if ((xBinarySemaphore != NULL) && (xMutexUART != NULL))
    {
        // Inicializacion del hardware (clocks, GPIOs, IRQs)
        prvSetupHardware();
        xQueue = xQueueCreate(QUEUE_SIZE, sizeof(axis));
        // Creacion de tarea RedLedTask
        xTaskCreate(prvHeartBeatTask, // Puntero a la funcion que implementa la tarea
                "HeartBeatTask",              // Nombre descriptivo de la tarea
                configMINIMAL_STACK_SIZE,  // Tama�o del stack de la tarea
                NULL,                      // Argumentos de la tarea
                HEARTBEAT_TASK_PRIORITY,  // Prioridad de la tarea
                NULL);
        // Creacion de tarea ReaderTask
        xTaskCreate(prvReaderTask, "ReaderTask",
        configMINIMAL_STACK_SIZE,
                    NULL,
                    READER_TASK_PRIORITY,
                    NULL);
        // Creacion de tarea ReaderTask
        xTaskCreate(prvUartWriterTask, "WriterTask",
        configMINIMAL_STACK_SIZE,
                    NULL,
                    UART_WRITER_TASK_PRIORITY,
                    NULL);
        // Puesta en marcha de las tareas creadas
        vTaskStartScheduler();
    }
    return 0;
}
// Tarea RedLedTask
static void prvHeartBeatTask(void *pvParameters)
{
    for (;;)
    {
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
        vTaskDelay(pdMS_TO_TICKS(HEART_BEAT_ON_MS));
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
        vTaskDelay(pdMS_TO_TICKS(HEART_BEAT_OFF_MS));
    }
}
// Tarea ReaderTask
static void prvReaderTask(void *pvParameters)
{
    // Tiempo maximo de espera entre dos interrupciones
    const TickType_t xMaxExpectedBlockTime = pdMS_TO_TICKS(500);
    for (;;)
    {
        axis *readAxis;
        readAxis = ADC_read();
        if ( xSemaphoreTake( xBinarySemaphore, xMaxExpectedBlockTime ) == pdPASS)
        {
            xQueueSendToBack(xQueue, readAxis, portMAX_DELAY);
            vTaskDelay(pdMS_TO_TICKS(DELAY_100_MS));
        }

    }
}

// Tarea UARTWRITERTask
static void prvUartWriterTask(void *pvParameters)
{
    char num[20];
    char message[100];
    for (;;)
    {
        axis writeAxis;
        while (xQueueReceive(xQueue, &writeAxis, 0))
        {
            strcpy(message, "\nAcelaracion en eje X: ");
            ftoa(writeAxis.x, num, 1);
            strcat(message, num);
            strcat(message, "g\n\rAcelaracion en eje Y: ");
            ftoa(writeAxis.y, num, 1);
            strcat(message, num);
            strcat(message, "g\n\rAcelaracion en eje Z: ");
            ftoa(writeAxis.z, num, 1);
            strcat(strcat(message, num), "g\n\r");
            xSemaphoreTake(xMutexUART,portMAX_DELAY);
            printf_(EUSCI_A0_BASE, message);
            xSemaphoreGive(xMutexUART);
        }
        vTaskDelay(pdMS_TO_TICKS(DELAY_500_MS));
    }
}
