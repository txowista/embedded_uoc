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
#include "accelerometer_driver.h"
#include "adc14_multiple_channel_no_repeat.h"
#include "LcdDriver/Crystalfontz128x128_ST7735.h"

// Definicion de parametros RTOS
#define WRITER_TASK_PRIORITY    1
#define READER_TASK_PRIORITY  2
#define HEARTBEAT_TASK_PRIORITY 1
#define QUEUE_SIZE  10
#define ONE_SECOND_MS  1000
#define DELAY_100_MS     100
#define DELAY_500_MS     500
#define HEART_BEAT_ON_MS 10
#define HEART_BEAT_OFF_MS 990
#define TEMP_SIZE 60

// Prototipos de funciones privadas
static void prvSetupHardware(void);
static void prvTempLightWriterTask(void *pvParameters);
static void prvForceGWriterTask(void *pvParameters);
static void prvReaderTask(void *pvParameters);
static void prvHeartBeatTask(void *pvParameters);
static TickType_t xBlinkOn;
static TickType_t xBlinkOff;
// Declaracion de un mutex para acceso unico a I2C, UART y buffer
SemaphoreHandle_t xMutexI2C;
SemaphoreHandle_t xMutexBuff;
SemaphoreHandle_t xMutexSPI;
SemaphoreHandle_t xMutexTempForceG;
// Declaracion de un semaforo binario para la lectura de ADC
volatile SemaphoreHandle_t xBinarySemaphore;

typedef enum Sensor
{
    light = 1, temp = 2
} Sensor;

// Queue element
typedef struct
{
    Sensor sensor;
    float value;
} Queue_reg_t;
// Tipo de mensaje a almacenar en la cola
typedef struct
{
    float acc_x;
    float acc_y;
    float acc_z;
} acc_t;
acc_t addAxis;
uint8_t sizeAxis;
QueueHandle_t xQueue; // Variable para referenciar a la cola
QueueHandle_t xQueueLightTemp; // Variable para referenciar a la cola
float addLight;
float previousTemp;
float addTemp;
float forceGTemp;
int sizeTemp;
int forceGTempCount;
/* Graphic library context */
Graphics_Context g_sContext;
//Funciones
void drawTitle(void);
void drawText(char *message, int pos);
void drawAxis(int size);
void setOrientation(acc_t *orientationAxis);
void drawLight(void);
void drawTemp(void);
void drawDiffTemp(void);
void accumulateAxis(acc_t accInReadAxis);
void accumulateLight(float addInLight);
void accumulateTemp(float addInTemp);
int main(void)
{
    // Inicializacion de semaforo binario
    xBinarySemaphore = xSemaphoreCreateBinary();
    // Inicializacio de mutexs
    xMutexI2C = xSemaphoreCreateMutex();
    xMutexBuff = xSemaphoreCreateMutex();
    xMutexSPI = xSemaphoreCreateMutex();
    xMutexTempForceG = xSemaphoreCreateMutex();
    xBlinkOff = pdMS_TO_TICKS(HEART_BEAT_OFF_MS);
    xBlinkOn = pdMS_TO_TICKS(HEART_BEAT_ON_MS);

    // Comprueba si semaforo y mutex se han creado bien
    if ((xBinarySemaphore != NULL) && (xMutexBuff != NULL)
            && (xMutexSPI != NULL) && (xMutexTempForceG != NULL)
            && (xMutexI2C != NULL))
    {
        // Inicializacion del hardware (clocks, GPIOs, IRQs)
        prvSetupHardware();
        sizeTemp = 0;
        forceGTempCount = 0;
        forceGTemp = 20;
        xQueue = xQueueCreate(QUEUE_SIZE, sizeof(acc_t));
        xQueueLightTemp = xQueueCreate(QUEUE_SIZE, sizeof(Queue_reg_t));
        // Creacion de tareas
        xTaskCreate(prvTempLightWriterTask, "TempLightWriterTask",
        configMINIMAL_STACK_SIZE,
                    NULL,
                    WRITER_TASK_PRIORITY,
                    NULL);
        xTaskCreate(prvForceGWriterTask, "ForceGWriterTask",
        configMINIMAL_STACK_SIZE,
                    NULL,
                    WRITER_TASK_PRIORITY,
                    NULL);
        xTaskCreate(prvReaderTask, "ReaderTask",
        configMEDIUM_STACK_SIZE,
                    NULL,
                    READER_TASK_PRIORITY,
                    NULL);
        xTaskCreate(prvHeartBeatTask, "HeartBeatTask",
        configMINIMAL_STACK_SIZE,
                    NULL,
                    HEARTBEAT_TASK_PRIORITY,
                    NULL);
        vTaskStartScheduler();
    }
    // Solo llega aqui si no hay suficiente memoria
    // para iniciar el scheduler
    return 0;
}

// Inicializacion del hardware del sistema
static void prvSetupHardware(void)
{

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

    /* Initializes display */
    Crystalfontz128x128_Init();

    /* Set default screen orientation */
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    /* Initializes graphics context */
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128,
                         &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);

    drawTitle();
    //Inicializa ADC
    init_ADC();
    // Habilita que el procesador responda a las interrupciones
    MAP_Interrupt_enableMaster();
}
/*
 * Clear display and redraw title + accelerometer data
 */
void drawTitle()
{
    Graphics_clearDisplay(&g_sContext);
    drawText("PRAC IGOR G.L.:", 10);

}
void drawText(char *message, int pos)
{
    if (xSemaphoreTake(xMutexSPI, portMAX_DELAY))
    {
        Graphics_drawStringCentered(&g_sContext, (int8_t *) message,
        AUTO_STRING_LENGTH,
                                    64, pos, OPAQUE_TEXT);
        xSemaphoreGive(xMutexSPI);
    }

}
void setOrientation(acc_t *orientationAxis)
{
    if (orientationAxis->acc_x < -0.2)
    {
        if (Lcd_Orientation != LCD_ORIENTATION_LEFT)
        {
            Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_LEFT);
            drawTitle();
        }
    }
    else if (orientationAxis->acc_x > 0.2)
    {
        if (Lcd_Orientation != LCD_ORIENTATION_RIGHT)
        {
            Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_RIGHT);
            drawTitle();
        }
    }
    else if (orientationAxis->acc_y > 0.2)
    {
        if (Lcd_Orientation != LCD_ORIENTATION_DOWN)
        {
            Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_DOWN);
            drawTitle();
        }
    }
    else
    {
        if (Lcd_Orientation != LCD_ORIENTATION_UP)
        {
            Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
            drawTitle();
        }
    }
}
void drawAxis(int size)
{
    char message[20];
    char value[10];
    acc_t myDrawAxis;
    myDrawAxis.acc_x = addAxis.acc_x / (float) size;
    myDrawAxis.acc_y = addAxis.acc_y / (float) size;
    myDrawAxis.acc_z = addAxis.acc_z / (float) size;
    setOrientation(&myDrawAxis);
    strcpy(message, "  Eje X: ");
    Accel_ftoa(myDrawAxis.acc_x, value, 2);
    strcat(message, value);
    drawText(strcat(message, "  "), 70);
    strcpy(message, "  Eje Y: ");
    Accel_ftoa(myDrawAxis.acc_y, value, 2);
    strcat(message, value);
    drawText(strcat(message, "  "), 90);
    strcpy(message, "  Eje Z: ");
    Accel_ftoa(myDrawAxis.acc_z, value, 2);
    strcat(message, value);
    drawText(strcat(message, "  "), 110);
    addAxis.acc_x = 0;
    addAxis.acc_y = 0;
    addAxis.acc_z = 0;
}
void accumulateAxis(acc_t accInReadAxis)
{
    addAxis.acc_x += accInReadAxis.acc_x;
    addAxis.acc_y += accInReadAxis.acc_y;
    addAxis.acc_z += accInReadAxis.acc_z;
}
void accumulateLight(float addInLight)
{
    if (addLight == 0)
    {
        addLight = addInLight;
    }
    else
    {
        addLight += addInLight;
        addLight /= 2.0;
        drawLight();
        addLight = 0;
    }
}
void drawLight(void)
{
    char message[20];
    char value[10];
    strcpy(message, "  Luz: ");
    ftoa(addLight, value, 2);
    strcat(message, value);
    drawText(strcat(message, "  "), 30);
}
void accumulateTemp(float addInTemp)
{
    if (xSemaphoreTake(xMutexTempForceG, portMAX_DELAY))
    {
        addTemp += addInTemp;
        sizeTemp++;
        if (sizeTemp >= TEMP_SIZE)
        {
            addTemp /= TEMP_SIZE;
            sizeTemp = 0;
            drawTemp();
            if (previousTemp != 0)
            {
                drawDiffTemp();
            }
            previousTemp = addTemp;
        }
        xSemaphoreGive(xMutexTempForceG);
    }
}
void drawTemp()
{
    char message[20];
    char value[10];
    strcpy(message, "  Temp: ");
    ftoa(addTemp, value, 2);
    strcat(message, value);
    drawText(strcat(message, "  "), 40);
}
void drawDiffTemp()
{
    char message[25];
    char value[10];
    strcpy(message, "  T Previous: ");
    Accel_ftoa(addTemp - previousTemp, value, 2);
    strcat(message, value);
    drawText(strcat(message, "  "), 50);
}

//Tarea heart beat
static void prvHeartBeatTask(void *pvParameters)
{
    for (;;)
    {
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
        vTaskDelay(xBlinkOn);
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
        vTaskDelay(xBlinkOff);
    }
}

//Tarea lectura temperatura y Luz
static void prvTempLightWriterTask(void *pvParameters)
{
    // Resultado del envio a la cola
    Queue_reg_t queueRegister;
    uint16_t rawData;
    float convertedLux;
    float temperature;
    int par = 0;
    for (;;)
    {
        par++;
        vTaskDelay(pdMS_TO_TICKS(DELAY_500_MS));
        // Intenta coger el mutex, bloqueandose si no esta disponible
        if (par >= 2)
        {
            xSemaphoreTake(xMutexI2C, portMAX_DELAY);
            {
                // Lee el valor de la medida de temperatura
                temperature = TMP006_readAmbientTemperature();
                queueRegister.sensor = temp;
                queueRegister.value = temperature;
                xSemaphoreGive(xMutexI2C);
            }
            if (xSemaphoreTake(xMutexBuff, portMAX_DELAY))
            {
                xQueueSend(xQueueLightTemp, (void * ) &queueRegister,
                           (TickType_t ) 0);
                xSemaphoreGive(xMutexBuff);
            }
            par = 0;
        }
        xSemaphoreTake(xMutexI2C, portMAX_DELAY);
        {
            // Lee el valor de la medida de luz
            sensorOpt3001Read(&rawData);
            sensorOpt3001Convert(rawData, &convertedLux);
            queueRegister.sensor = light;
            queueRegister.value = convertedLux;
            xSemaphoreGive(xMutexI2C);
        }
        if (xSemaphoreTake(xMutexBuff, portMAX_DELAY))
        {
            xQueueSend(xQueueLightTemp, (void * ) &queueRegister,
                       (TickType_t ) 0);
            xSemaphoreGive(xMutexBuff);
        }
    }
}

//Tarea lectura ADC
static void prvForceGWriterTask(void *pvParameters)
{
    // Resultado del envio a la cola
    acc_t queue_element;
    float Datos[NUM_ADC_CHANNELS];
    xSemaphoreTake(xBinarySemaphore, 0);
    // La tarea se repite en un bucle infinito
    for (;;)
    {
        forceGTempCount++;
        if (forceGTempCount >= 30)
        {
            forceGTempCount = 0;
            if (xSemaphoreTake(xMutexTempForceG, portMAX_DELAY))
            {
                if (sizeTemp != 0)
                {
                    forceGTemp = addTemp / sizeTemp;
                }
                else
                {
                    forceGTemp = addTemp;
                }
                xSemaphoreGive(xMutexTempForceG);
            }
        }
        Accel_read(Datos);
        queue_element.acc_x = Datos[0];
        queue_element.acc_y = Datos[1];
        queue_element.acc_z = Datos[2];

        xQueueSend(xQueue, &queue_element, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(DELAY_100_MS));
    }
}

//Tarea lectura de colas
static void prvReaderTask(void *pvParameters)
{
    Queue_reg_t queueRegister;
    acc_t queue_element;
    sizeAxis = 0;
    int size = 0;
    queue_element.acc_x = 0;
    queue_element.acc_y = 0;
    queue_element.acc_z = 0;
    for (;;)
    {
        size = 0;
        vTaskDelay(pdMS_TO_TICKS(ONE_SECOND_MS));
        if (xSemaphoreTake(xMutexBuff, portMAX_DELAY))
        {
            while (xQueueReceive(xQueueLightTemp, &queueRegister,
                                 (TickType_t ) 10))
            {
                if (queueRegister.sensor == light)
                {
                    accumulateLight(queueRegister.value);
                }
                else if (queueRegister.sensor == temp)
                {
                    accumulateTemp(queueRegister.value);
                }

            }
            xSemaphoreGive(xMutexBuff);
        }

        size = uxQueueMessagesWaiting(xQueue);
        while (xQueueReceive(xQueue, &queue_element, 0))
        {
            accumulateAxis(queue_element);
        }
        drawAxis(size);

    }
}

