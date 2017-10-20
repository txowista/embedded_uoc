// Includes standard
#include <stdio.h>
#include <stdint.h>

// Includes FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
// Includes drivers
#include "uart_driver.h"
#include "timer_A1.h"
#include "i2c_driver.h"
#include "temp_sensor.h"
#include "opt3001.h"
#include "tmp006.h"

// Prototipos de funciones privadas
static void prvSetupHardware(void);
// Definicion de prioridades de tareas
#define prvLED_TASK_PRIORITY    3
static void prvRedLedTask(void *pvParameters);

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
    CS_setDCOFrequency(8000000);

    // Inicializa los clocks HSMCLK, SMCLK, MCLK y ACLK
    MAP_CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // Selecciona el nivel de tension del core
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE0);

    // Inicializacion del I2C
    I2C_init();

    // Inicializacion de la UART
//    UartInit(OnUart_rxChar);

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
}


/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
    // Inicializacion del hardware (clocks, GPIOs, IRQs)
    prvSetupHardware();
    // Creacion de tarea RedLedTask
    xTaskCreate( prvRedLedTask,            // Puntero a la funcion que implementa la tarea
                "RedLedTask",              // Nombre descriptivo de la tarea
                configMINIMAL_STACK_SIZE,  // Tama�o del stack de la tarea
                NULL,                      // Argumentos de la tarea
                prvLED_TASK_PRIORITY,  // Prioridad de la tarea
                NULL );

    // Puesta en marcha de las tareas creadas
    vTaskStartScheduler();
}
// Tarea RedLedTask
static void prvRedLedTask (void *pvParameters)
{
    // Calcula el tiempo de activacion del LED (en ticks)
    // a partir del tiempo en milisegundos
    static const TickType_t xBlinkOn  = pdMS_TO_TICKS(10);

    // Calcula el tiempo de desactivacion del LED (en ticks)
    // a partir del tiempo en milisegundos
    static const TickType_t xBlinkOff = pdMS_TO_TICKS(990);

    // La tarea se repite en un bucle infinito
    for(;;) {
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
