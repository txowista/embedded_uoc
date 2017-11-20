//*****************************************************************************
//
// Copyright (C) 2015 - 2016 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//  Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the
//  distribution.
//
//  Neither the name of Texas Instruments Incorporated nor the names of
//  its contributors may be used to endorse or promote products derived
//  from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

// Includes standard
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Includes FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// Includes drivers
#include "i2c_driver.h"
#include "temp_sensor.h"
#include "opt3001.h"
#include "tmp006.h"

// Definicion de parametros RTOS
#define WRITER_TASK_PRIORITY    1
#define READER_TASK_PRIORITY  2
#define HEARTBEAT_TASK_PRIORITY 1
#define QUEUE_SIZE  10
#define ONE_SECOND_MS  1000
#define HEART_BEAT_ON_MS 10
#define HEART_BEAT_OFF_MS 990
#define BUFFER_SIZE 10

#define DEBUG_MSG 0

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

// Prototipos de funciones privadas
static void prvSetupHardware(void);
static void prvTempWriterTask(void *pvParameters);
static void prvLightWriterTask(void *pvParameters);
static void prvReaderTask(void *pvParameters);
static void prvHeartBeatTask(void *pvParameters);

// Declaracion de un mutex para acceso unico a I2C, UART y buffer
SemaphoreHandle_t xMutexI2C;
SemaphoreHandle_t xMutexUART;
SemaphoreHandle_t xMutexBuff;
// Declaracion de un semaforo binario para la ISR/lectura de buffer
SemaphoreHandle_t xBinarySemaphoreISR;

typedef enum Sensor
{
    light = 1,
    temp = 2
} Sensor;

// Queue element
typedef struct {
    Sensor sensor;
    float value;
} Queue_reg_t;

// Buffer de mensajes
Queue_reg_t buffer[BUFFER_SIZE];
uint8_t buff_pos;

//funcion imprimir string por UART
void printf_(uint32_t moduleInstance, char *message){
    int index = 0;
    while(message[index]!='\0'){
        MAP_UART_transmitData(EUSCI_A0_BASE, message[index]);
        index++;
    }
}

int main(void)
{
    // Inicializacion de semaforo binario
    xBinarySemaphoreISR = xSemaphoreCreateBinary();
    // Inicializacio de mutexs
    xMutexI2C = xSemaphoreCreateMutex();
    xMutexUART = xSemaphoreCreateMutex();
    xMutexBuff= xSemaphoreCreateMutex();



    // Comprueba si semaforo y mutex se han creado bien
    if ((xBinarySemaphoreISR != NULL) && (xMutexBuff != NULL) &&
            (xMutexI2C != NULL) && (xMutexUART != NULL))
    {
        // Inicializacion del hardware (clocks, GPIOs, IRQs)
        prvSetupHardware();
        // Inicializacion buffer
        for(int i=0; i<BUFFER_SIZE;i++){
            buffer[i].sensor = light;
            buffer[i].value = -1.0;
        }
        buff_pos = 0;

        // Creacion de tareas
        xTaskCreate( prvTempWriterTask, "TempWriterTask", configMINIMAL_STACK_SIZE, NULL,
                     WRITER_TASK_PRIORITY,NULL );
        xTaskCreate( prvLightWriterTask, "LightWriterTask", configMINIMAL_STACK_SIZE, NULL,
                    WRITER_TASK_PRIORITY, NULL );
        xTaskCreate( prvReaderTask, "ReaderTask", configMINIMAL_STACK_SIZE, NULL,
                    READER_TASK_PRIORITY, NULL );
        xTaskCreate( prvHeartBeatTask, "HeartBeatTask", configMINIMAL_STACK_SIZE, NULL,
                     HEARTBEAT_TASK_PRIORITY, NULL );
        // Puesta en marcha de las tareas creadas
        printf_(EUSCI_A0_BASE, "Tareas creadas \n\r");
        vTaskStartScheduler();
    }
    // Solo llega aqui si no hay suficiente memoria
    // para iniciar el scheduler
    return 0;
}

// Inicializacion del hardware del sistema
static void prvSetupHardware(void){

    extern void FPU_enableModule(void);

    // Configuracion del pin P1.0 - LED 1 - como salida y puesta en OFF
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

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

    // Inicializacion del I2C
    I2C_init();
    // Inicializacion del sensor TMP006
    TMP006_init();
    // Inicializacion del sensor opt3001
    sensorOpt3001Init();
    sensorOpt3001Enable(true);

    // Configuracion del pin P1.1 como entrada con R de pull-up
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1, GPIO_PIN1);

    // Seleccion de modo UART en pines P1.2 y P1.3
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    // Configuracion de la UART
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);
    // Habilitacion de la UART
    MAP_UART_enableModule(EUSCI_A0_BASE);


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
}

//Tarea heart beat
static void prvHeartBeatTask (void *pvParameters){
    for(;;){
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
        vTaskDelay( pdMS_TO_TICKS(HEART_BEAT_ON_MS) );
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
        vTaskDelay( pdMS_TO_TICKS(HEART_BEAT_OFF_MS) );
    }
}

//Tarea lectura temperatura
static void prvTempWriterTask (void *pvParameters){
    // Resultado del envio a la cola
    Queue_reg_t queueRegister;

    float temperature;

    for(;;){
        vTaskDelay( pdMS_TO_TICKS(ONE_SECOND_MS) );

        // Lee el valor de la medida de temperatura
        temperature = TMP006_readAmbientTemperature();

        queueRegister.sensor = temp;
        queueRegister.value = temperature;
        // Envia un comando a traves de la cola si hay espacio
        if (DEBUG_MSG && xSemaphoreTake(xMutexUART,portMAX_DELAY)){
            printf_(EUSCI_A0_BASE, "Enviando temperatura... \n");
            xSemaphoreGive(xMutexUART);
        }
        if(xSemaphoreTake(xMutexBuff,portMAX_DELAY)){
            buff_pos++;
            buffer[buff_pos % BUFFER_SIZE] = queueRegister;
            xSemaphoreGive(xMutexBuff);
        }

    }
}

//Tarea lectura luz
static void prvLightWriterTask (void *pvParameters){

    // Resultado del envio a la cola
    Queue_reg_t queueRegister;

    uint16_t rawData;
    float convertedLux;

    for(;;){
        vTaskDelay(pdMS_TO_TICKS(ONE_SECOND_MS));

        // Lee el valor de la medida de luz
        sensorOpt3001Read(&rawData);
        sensorOpt3001Convert(rawData, &convertedLux);

        queueRegister.sensor = light;
        queueRegister.value = convertedLux;
        // Envia un comando a traves de la cola si hay espacio
        if(DEBUG_MSG && xSemaphoreTake(xMutexUART,portMAX_DELAY)){
            printf_(EUSCI_A0_BASE, "Enviando luz... \n");
            xSemaphoreGive(xMutexUART);
        }
        if(xSemaphoreTake(xMutexBuff,portMAX_DELAY)){
            buff_pos++;
            buffer[buff_pos % BUFFER_SIZE] = queueRegister;
            xSemaphoreGive(xMutexBuff);
        }
    }
}

//Tarea lectura de cola
static void prvReaderTask (void *pvParameters)
{
    Queue_reg_t queueRegister;
    char message[50];
    char value[10];
    for(;;){
        // El semaforo debe ser entregado por la ISR PORT1_IRQHandler
        // Espera un numero maximo de xMaxExpectedBlockTime ticks
        if( xSemaphoreTake( xBinarySemaphoreISR, portMAX_DELAY ) == pdPASS ){
            if (xSemaphoreTake(xMutexUART,portMAX_DELAY)){
                printf_(EUSCI_A0_BASE, "\n\r");
                xSemaphoreGive(xMutexUART);
            }
            if(xSemaphoreTake(xMutexBuff,portMAX_DELAY)){
                for(int i=0; i<BUFFER_SIZE; i++){
                    queueRegister = buffer[i];
                    if (queueRegister.sensor==light){
                        strcpy(message, "Lectura de luz: ");
                    }else if(queueRegister.sensor==temp){
                        strcpy(message,"Lectura de temperatura: ");
                    }
                    ftoa(queueRegister.value, value, 2);
                    strcat(strcat(message,value),"\n\r");
                    if(xSemaphoreTake(xMutexUART,portMAX_DELAY)){
                        printf_(EUSCI_A0_BASE, message);
                        xSemaphoreGive(xMutexUART);
                    }
                }
                xSemaphoreGive(xMutexBuff);
            }
        }
    }
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
            // El par�metro xHigherPriorityTaskWoken debe inicializarse en pdFALSE,
            // ya que se establecer� en pdTRUE dentro de la funci�n API de interrupci�n
            // segura si se requiere un cambio de contexto
            xHigherPriorityTaskWoken = pdFALSE;

            // Entrega el semaforo desde la ISR para desbloquear la tarea SenderTask
            xSemaphoreGiveFromISR( xBinarySemaphoreISR, &xHigherPriorityTaskWoken );

            // Pasa el valor xHigherPriorityTaskWoken en portYIELD_FROM_ISR().
            // Si xHigherPriorityTaskWoken se estableci� en pdTRUE dentro de
            // xSemaphoreGiveFromISR(), entonces al llamar a portYIELD_FROM_ISR()
            // solicitar� un cambio de contexto. Si xHigherPriorityTaskWoken sigue
            // siendo pdFALSE, entonces la llamada a portYIELD_FROM_ISR() no tendr�
            // ning�n efecto
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
        }
}
