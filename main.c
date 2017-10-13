//*****************************************************************************
//
// Copyright (C) 2017 Universitat Oberta de Cataluya
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

// Include DriverLib (MSP432 Peripheral Driver Library)
#include "driverlib.h"
/* Standard Includes */
#include <stdio.h>
#include <string.h>
volatile unsigned int S1Debounce = 0; // Deboounce state for button S1
volatile unsigned int S2Debounce = 0; // Deboounce state for button S2
volatile int countT2 = 0;
volatile int countT3 = 0;
// Timer_A UpMode Configuration
const Timer_A_UpModeConfig upConfig = {
TIMER_A_CLOCKSOURCE_SMCLK,          // SMCLK source = 3 MHz
        TIMER_A_CLOCKSOURCE_DIVIDER_64,     // SMCLK/64
        46875,                       // 1 seconds
        TIMER_A_TAIE_INTERRUPT_DISABLE,     // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE, // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                    // Clear value
        };
const Timer_A_UpModeConfig upConfigDebounce = {
TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_1,          // SMCLK/1 = 3MHz
        30000,                                  // 10ms debounce period
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
        };
const Timer_A_UpModeConfig upConfigTimer2= {
TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_64,          // SMCLK/1 = 3MHz
        4687,                                  // 100ms
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
        };
const Timer_A_UpModeConfig upConfigTimer3= {
TIMER_A_CLOCKSOURCE_SMCLK,              // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_64,          // SMCLK/1 = 3MHz
        9374,                                  // 100ms
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
        };
// UART Configuration Parameter (9600 bps)
const eUSCI_UART_Config uartConfig = {
EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        78,                                      // BRDIV = 78
        2,                                       // UCxBRF = 2
        0,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
        };

int main(void)
{

    // Configuracion del pin P1.0 como salida
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    // Fijar a 0 la salida del pin P1.0
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    // Configuracion del pin P2.0 como salida
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2,   GPIO_PIN0 | GPIO_PIN1 |GPIO_PIN2);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN0 |  GPIO_PIN1 |GPIO_PIN2);

    /*TIMER A0*/
    MAP_Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);
    MAP_Interrupt_enableInterrupt(INT_TA0_0);
    MAP_Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
    /*TIMER A1*/
    MAP_Timer_A_configureUpMode(TIMER_A1_BASE, &upConfigDebounce);
    MAP_Interrupt_enableInterrupt(INT_TA1_0);
    /*TIMER A2*/
    MAP_Timer_A_configureUpMode(TIMER_A2_BASE, &upConfigTimer2);
    MAP_Interrupt_enableInterrupt(INT_TA2_0);
    /**CONFIG UART*/
    // Seleccion de modo UART en pines P1.2 y P1.3
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);
    MAP_UART_enableModule(EUSCI_A0_BASE);

    // Habilita el procesador para que duerma al acabar la ISR
    MAP_Interrupt_enableSleepOnIsrExit();

    // Habilita al procesador para que responda a interrupciones
    MAP_Interrupt_enableMaster();


    /* Confinguring P1.1 & P1.4 as an input and enabling interrupts */
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P1,
    GPIO_PIN1 | GPIO_PIN4);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4);
    MAP_GPIO_interruptEdgeSelect(GPIO_PORT_P1, GPIO_PIN1 | GPIO_PIN4,
    GPIO_HIGH_TO_LOW_TRANSITION);
    MAP_Interrupt_enableInterrupt(INT_PORT1);

    while (1)
    {
        // Conmuta el microcontrolador a modo de bajo consumo LPM0
        MAP_PCM_gotoLPM0();
    }
}

// Rutina de Servicio a Interrupcion (ISR) del Timer_A1
void TA0_0_IRQHandler(void)
{
    // Conmuta el estado de la salida digital P1.0 (LED)
    MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
    // Resetea el flag de interrupcion del Timer_A1
    MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,
    TIMER_A_CAPTURECOMPARE_REGISTER_0);
}
void TA1_0_IRQHandler(void)
{
//    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    if (P1IN & GPIO_PIN1)
    {
        S1Debounce = 0;
    }
    if (P1IN & GPIO_PIN4)
    {
        S2Debounce = 0;
    }

    if ((P1IN & GPIO_PIN1) && (P1IN & GPIO_PIN4))
    {
        MAP_Timer_A_stopTimer(TIMER_A1_BASE);
    }
    MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE,
    TIMER_A_CAPTURECOMPARE_REGISTER_0);
}
// Rutina de Servicio a Interrupcion (ISR) del PORT1
void PORT1_IRQHandler(void)
{
    uint32_t status;
    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P1);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P1, status);

    // Chequea si la interrupcion la genero el pin P1.1
    if (status & GPIO_PIN1)
    {
        if (S1Debounce == 0)
        {
            S1Debounce = 1;
            MAP_UART_transmitData(EUSCI_A0_BASE,'S');
            MAP_UART_transmitData(EUSCI_A0_BASE,'1');
            /*TIMER A2*/
            MAP_Timer_A_configureUpMode(TIMER_A2_BASE, &upConfigTimer2);
            MAP_Interrupt_enableInterrupt(INT_TA2_0);
            MAP_Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);

        }
        MAP_Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
    }
    /* Handles S2 button press */
    if (status & GPIO_PIN4)
    {
        if (S2Debounce == 0)
        {
            S2Debounce = 1;
            MAP_UART_transmitData(EUSCI_A0_BASE,'S');
            MAP_UART_transmitData(EUSCI_A0_BASE,'2');
            /*TIMER A3*/
            MAP_Timer_A_configureUpMode(TIMER_A3_BASE, &upConfigTimer3);
            MAP_Interrupt_enableInterrupt(INT_TA3_0);
            MAP_Timer_A_startCounter(TIMER_A3_BASE, TIMER_A_UP_MODE);

        }
        MAP_Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);
    }

}
void TA2_0_IRQHandler(void)
{
    if (countT2 < 10)
    {
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);
        countT2++;
        MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A2_BASE,
        TIMER_A_CAPTURECOMPARE_REGISTER_0);
    }
    else
    {
        countT2 = 0;
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,   GPIO_PIN0 | GPIO_PIN1 |GPIO_PIN2);
        MAP_Interrupt_disableInterrupt(INT_TA2_0);
        MAP_Timer_A_stopTimer(TIMER_A2_BASE);
    }
}
void TA3_0_IRQHandler(void)
{
    if (countT3 < 10)
    {
        MAP_GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN2);
        countT3++;
        MAP_Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE,
        TIMER_A_CAPTURECOMPARE_REGISTER_0);
    }
    else
    {
        countT3 = 0;
        MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P2,   GPIO_PIN0 | GPIO_PIN1 |GPIO_PIN2);
        MAP_Interrupt_disableInterrupt(INT_TA3_0);
        MAP_Timer_A_stopTimer(TIMER_A3_BASE);
    }
}

