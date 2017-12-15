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

// Includes standard C
#include <stdio.h>
#include <stdint.h>
#include <string.h>

// Includes FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// Includes drivers peripherical
#include "i2c_driver.h"
#include "temp_sensor.h"
#include "opt3001.h"
#include "tmp006.h"
#include "accelerometer_driver.h"
#include "adc14_multiple_channel_no_repeat.h"
#include "LcdDriver/Crystalfontz128x128_ST7735.h"

// Definicion de parametros
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
#define FORCEG_TEMP_SIZE 300
#define TEMP_INIT_FORCEG 20
#define ROTATION_THRESHOLD 0.2
#define TITLE_LINE 10
#define LIGHT_LINE 30
#define TEMP_LINE 40
#define PREVIOUS_TEMP_LINE 50
#define EJEX_LINE 70
#define EJEY_LINE 90
#define EJEZ_LINE 110
#define NUMBER_OF_DECIMAL 2

static TickType_t xBlinkOn;
static TickType_t xBlinkOff;
// Declaracion de un mutex para acceso unico a I2C, UART ,buffer y TempForceG
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
} Queue_reg_t; // Tipo de mensaje a almacenar en la cola
typedef struct
{
    float acc_x;
    float acc_y;
    float acc_z;
} acc_t;
acc_t addAxis;
QueueHandle_t xQueue; // Variable para referenciar a la cola, de los valores de la aceleracion
QueueHandle_t xQueueLightTemp; // Variable para referenciar a la cola, de los valores de temperatura y luz
float addLight; //Variable global acumulador para los datos de Luz
float previousTemp; //Variable global para la temperatura previa
float addTemp; //Variable global acumulador para los datos de temperatura
float forceGTemp; //Variable global temperatura correccion datos del acelerometro
uint8_t tempCount; //Contador para el acumulador de temperatura
uint8_t forceGTempCount; //Contador para el acumulador de temperatura correccion datos del acelerometro
Graphics_Context g_sContext; //Contexto para la libreria grafica
// Prototipos de funciones privadas
static void prvSetupHardware(void); //Seteo de hardware
static void prvTempLightWriterTask(void *pvParameters); //Tarea obtencion datos de Luz y Temperatura
static void prvForceGWriterTask(void *pvParameters); //Tarea obtencion datos de aceleracion
static void prvReaderTask(void *pvParameters); //Tarea lectura de datos de las colas para su impresion en pantalla
static void prvHeartBeatTask(void *pvParameters); //Tarea de latido del led
static void drawTitle(void); //Presenta en pantalla el titulo en pantalla
static void drawText(char *message, int pos); //Presenta en pantalla en funcion de un texto y una posicion
static void drawAxis(int size); //Presenta en pantalla datos del acelerometro
static void setOrientation(acc_t *orientationAxis); //Cambia orientacion de pantalla en funcion de datos del acelerometro
static void drawLight(void); //Presenta en pantalla media del sensor de luz
static void drawTemp(void); //Presenta en pantalla media del sensor de temperatura
static void drawDiffTemp(void); //Presenta en pantalla temperatura anterior
static void summatoryAxis(acc_t accInReadAxis); //Acumula los datos de la cola del acelerometro para obtener media
static void summatoryLight(float addInLight); //Acumula los datos de la cola del sensor de luz para obtener media
static void summatoryTemp(float addInTemp); //Acumula los datos dela cola del sensor de temperatura para obtener media
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

    // Comprueba si semaforo y mutexs se han creado bien
    if ((xBinarySemaphore != NULL) && (xMutexBuff != NULL)
            && (xMutexSPI != NULL) && (xMutexTempForceG != NULL)
            && (xMutexI2C != NULL))
    {
        // Inicializacion del hardware (clocks, GPIOs, IRQs)
        prvSetupHardware();
        //Seteo de varibales
        tempCount = 0;
        forceGTempCount = 0;
        forceGTemp = TEMP_INIT_FORCEG;
        //Creacion de las colas
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
        configMEDIUM_STACK_SIZE, //Necesario mayor tamaño de stack debido a la carga de memoria
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

    //Inicializacion del display
    Crystalfontz128x128_Init();

    //Orientacion por defecto del display
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);
    //Inicializacion contexto libreria grafica
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128,
                         &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);

    //Inicializa ADC
    init_ADC();
    // Habilita que el procesador responda a las interrupciones
    MAP_Interrupt_enableMaster();
    //Mostramos en pantalla el titulo
    drawTitle();
}

static void drawTitle()
{
    Graphics_clearDisplay(&g_sContext);
    drawText("PRAC IGOR G.L.:", 10);

}
static void drawText(char *message, int pos)
{
    if (xSemaphoreTake(xMutexSPI, portMAX_DELAY))
    {
        Graphics_drawStringCentered(&g_sContext, (int8_t *) message,
        AUTO_STRING_LENGTH,
                                    64, pos, OPAQUE_TEXT);
        xSemaphoreGive(xMutexSPI);
    }

}
static void setOrientation(acc_t *orientationAxis)
{
    if (orientationAxis->acc_x < -ROTATION_THRESHOLD)
    {
        if (Lcd_Orientation != LCD_ORIENTATION_LEFT)
        {
            Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_LEFT);
            drawTitle();
        }
    }
    else if (orientationAxis->acc_x > ROTATION_THRESHOLD)
    {
        if (Lcd_Orientation != LCD_ORIENTATION_RIGHT)
        {
            Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_RIGHT);
            drawTitle();
        }
    }
    else if (orientationAxis->acc_y > ROTATION_THRESHOLD)
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
static void drawAxis(int size)
{
    char message[20];
    char value[10];
    acc_t myDrawAxis;
    //Obtencion media
    myDrawAxis.acc_x = addAxis.acc_x / (float) size;
    myDrawAxis.acc_y = addAxis.acc_y / (float) size;
    myDrawAxis.acc_z = addAxis.acc_z / (float) size;
    //Orientacion pantalla
    setOrientation(&myDrawAxis);
    //Formateo de textos
    strcpy(message, "  Eje X: ");
    Accel_ftoa(myDrawAxis.acc_x, value, NUMBER_OF_DECIMAL);
    strcat(message, value);
    drawText(strcat(message, "  "), EJEX_LINE);
    strcpy(message, "  Eje Y: ");
    Accel_ftoa(myDrawAxis.acc_y, value, NUMBER_OF_DECIMAL);
    strcat(message, value);
    drawText(strcat(message, "  "), EJEY_LINE);
    strcpy(message, "  Eje Z: ");
    Accel_ftoa(myDrawAxis.acc_z, value, NUMBER_OF_DECIMAL);
    strcat(message, value);
    drawText(strcat(message, "  "), EJEZ_LINE);
    addAxis.acc_x = 0;
    addAxis.acc_y = 0;
    addAxis.acc_z = 0;
}
static void summatoryAxis(acc_t accInReadAxis)
{
    addAxis.acc_x += accInReadAxis.acc_x;
    addAxis.acc_y += accInReadAxis.acc_y;
    addAxis.acc_z += accInReadAxis.acc_z;
}
static void summatoryLight(float addInLight)
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
static void drawLight(void)
{
    char message[20];
    char value[10];
    strcpy(message, "  Luz: ");
    ftoa(addLight, value, 2);
    strcat(message, value);
    drawText(strcat(message, "  "), LIGHT_LINE);
}
static void summatoryTemp(float addInTemp)
{
    if (xSemaphoreTake(xMutexTempForceG, portMAX_DELAY))
    {
        addTemp += addInTemp;
        tempCount++;
        //Obtencion de 60 muestras antes de presentarlas en pantallas
        if (tempCount >= TEMP_SIZE)
        {
            addTemp /= TEMP_SIZE;
            tempCount = 0;
            drawTemp();
            //Presentacion de la temperatura previa
            if (previousTemp != 0)
            {
                drawDiffTemp();
            }
            previousTemp = addTemp;
        }
        xSemaphoreGive(xMutexTempForceG);
    }
}
static void drawTemp()
{
    char message[20];
    char value[10];
    strcpy(message, "  Temp: ");
    ftoa(addTemp, value, NUMBER_OF_DECIMAL);
    strcat(message, value);
    drawText(strcat(message, "  "), TEMP_LINE);
}
static void drawDiffTemp()
{
    char message[25];
    char value[10];
    strcpy(message, "  T Previous: ");
    Accel_ftoa(addTemp - previousTemp, value, NUMBER_OF_DECIMAL);
    strcat(message, value);
    drawText(strcat(message, "  "), PREVIOUS_TEMP_LINE);
}

//Tarea heart beat
static void prvHeartBeatTask(void *pvParameters)
{
    for (;;)
    {
        //Cambiamos el estado de la GPIO
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
    uint8_t par = 0;
    for (;;)
    {
        par++;
        vTaskDelay(pdMS_TO_TICKS(DELAY_500_MS));
        if (par >= 2)
        {
            // Intenta coger el mutex, bloqueandose si no esta disponible
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
    // Intenta coger el semaforo, bloqueandose si no esta disponible
    xSemaphoreTake(xBinarySemaphore, 0);
    // La tarea se repite en un bucle infinito
    for (;;)
    {
        forceGTempCount++;
        if (forceGTempCount >= FORCEG_TEMP_SIZE)
        {
            forceGTempCount = 0;
            // Intenta coger el mutex, bloqueandose si no esta disponible
            if (xSemaphoreTake(xMutexTempForceG, portMAX_DELAY))
            {
                if (tempCount != 0)
                {
                    forceGTemp = addTemp / tempCount;
                }
                else
                {
                    forceGTemp = addTemp;
                }
                xSemaphoreGive(xMutexTempForceG);
            }
        }
        //Funcion que recoge los valores del acelerometro
        Accel_read(Datos);
        queue_element.acc_x = Datos[x];
        queue_element.acc_y = Datos[y];
        queue_element.acc_z = Datos[z];
        //Insercion en la cola
        xQueueSend(xQueue, &queue_element, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(DELAY_100_MS));
    }
}

//Tarea lectura de colas
static void prvReaderTask(void *pvParameters)
{
    Queue_reg_t queueRegister;
    acc_t queue_element;
    UBaseType_t size = 0;
    for (;;)
    {
        size = 0;
        vTaskDelay(pdMS_TO_TICKS(ONE_SECOND_MS));
        if (xSemaphoreTake(xMutexBuff, portMAX_DELAY))
        {
            //Recorrido de todos los elementos de la cola
            while (xQueueReceive(xQueueLightTemp, &queueRegister,
                                 (TickType_t ) 10))
            {
                if (queueRegister.sensor == light)
                {
                    summatoryLight(queueRegister.value);
                }
                else if (queueRegister.sensor == temp)
                {
                    summatoryTemp(queueRegister.value);
                }

            }
            xSemaphoreGive(xMutexBuff);
        }
        //Obtenemos el tamaño de la cola para calcular la media
        size = uxQueueMessagesWaiting(xQueue);
        //Recorrido de todos los elementos de la cola
        while (xQueueReceive(xQueue, &queue_element, 0))
        {
            summatoryAxis(queue_element);
        }
        drawAxis((int) size);

    }
}

