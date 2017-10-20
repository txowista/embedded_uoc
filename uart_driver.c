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

// Includes standard
#include <stdbool.h>
#include <stdint.h>

// Include driver
#include "uart_driver.h"

// Include DriverLib (MSP432 Peripheral Driver Library)
#include "driverlib.h"

// Tamaño de los buffers FIFO de la UART (en bytes)
#define FIFO_TX_SIZE        512
#define FIFO_RX_SIZE        512

// Numero de veces que UartPrint intenta enviar al buffer de transmision
#define TX_BUFFER_RETRY_COUNT                       10

// UART Configuration Parameter (9600 bps)
const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        78,                                     // BRDIV = 78
        2,                                       // UCxBRF = 2
        0,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};

// Estructura de la FIFO
typedef struct Fifo_s
{
    uint16_t Begin;
    uint16_t End;
    uint8_t *Data;
    uint16_t Size;
}Fifo_t;


// FIFOs de transmision y recepcion
Fifo_t FifoTx;
Fifo_t FifoRx;

uint8_t UartTxBuffer[FIFO_TX_SIZE];
uint8_t UartRxBuffer[FIFO_RX_SIZE];

Uart_rxChar_cb_t Uart_rxCharCb;

// Prototipos de funciones privadas

// Envía un caracter a la UART
uint8_t UartPutChar( uint8_t data );

// Inicializa la FIFO
void FifoInit( Fifo_t *fifo, uint8_t *buffer, uint16_t size );

// Escribe un dato en la FIFO
void FifoPush( Fifo_t *fifo, uint8_t data );

// Lee un dato de la FIFO
uint8_t FifoPop( Fifo_t *fifo );

// Comprueba si la FIFO está vacía
bool IsFifoEmpty( Fifo_t *fifo );

// Comprueba si la FIFO está llena
bool IsFifoFull( Fifo_t *fifo );

// Lee la siguiente posición de la FIFO
static uint16_t FifoNext( Fifo_t *fifo, uint16_t index );

// Funciones publicas

// Inicializacion de la UART
void UartInit( Uart_rxChar_cb_t rxCharCb )
{
    // Seleccion de modo UART en pines P1.2 y P1.3
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
            GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    // Configuracion de la frecuencia del DCO a 12 MHz
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

    // Configuracion de la UART
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);

    // Habilitacion de la UART
    MAP_UART_enableModule(EUSCI_A0_BASE);

    // Habilita la interrupcion de recepcion
    MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    // Habilita la interrupcion de la UART
    MAP_Interrupt_enableInterrupt(INT_EUSCIA0);
    // Habilita el procesador para que duerma al acabar la ISR
    // MAP_Interrupt_enableSleepOnIsrExit();
    // Habilita al procesador para que responda a interrupciones
    MAP_Interrupt_enableMaster();

    // Inicializacion de las FIFO's de transmision y recepcion
    FifoInit( &FifoTx, UartTxBuffer, FIFO_TX_SIZE );
    FifoInit( &FifoRx, UartRxBuffer, FIFO_TX_SIZE );

    // Inicializa la funcion call-back de recepcion de caracter en la UART
    Uart_rxCharCb = rxCharCb;
}

// Envia una cadena de caracteres a traves de la UART
uint8_t UartPrint( char *s )
{
    uint8_t retryCount;

    while(*s)
    {
        retryCount = 0;
        while( UartPutChar( (uint8_t)*s ) != 0 )
        {
            retryCount++;

            if( retryCount > TX_BUFFER_RETRY_COUNT )
            {
                return 1; // Error
            }
        }
        s++;
    }

    return 0; // OK

}

// Funciones privadas


// Escribe un byte en la FIFO de transmision y habilita la interrupcion de transmision
uint8_t UartPutChar( uint8_t data )
{
    if( IsFifoFull( &FifoTx ) == false )
    {
        __disable_irq( );

        FifoPush( &FifoTx, data );

        __enable_irq( );

        // Habilita la interrupcion de transmision de la UART
        MAP_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT);
        MAP_Interrupt_enableInterrupt(INT_EUSCIA0);

        return 0; // OK
    }
    return 1; // Busy

}

// Lee un byte de la FIFO de recepcion
uint8_t UartGetChar( char *data )
{
    if( IsFifoEmpty( &FifoRx ) == false )
    {
        __disable_irq( );
        *data = FifoPop( &FifoRx );
        __enable_irq( );
        return 0;
    }
    return 1;
}

// Inicializa una FIFO
void FifoInit( Fifo_t *fifo, uint8_t *buffer, uint16_t size )
{
    fifo->Begin = 0;
    fifo->End = 0;
    fifo->Data = buffer;
    fifo->Size = size;
}

// Escribe un byte en la FIFO
void FifoPush( Fifo_t *fifo, uint8_t data )
{
    fifo->End = FifoNext( fifo, fifo->End );
    fifo->Data[fifo->End] = data;
}

// Lee un byte de la FIFO
uint8_t FifoPop( Fifo_t *fifo )
{
    uint8_t data = fifo->Data[FifoNext( fifo, fifo->Begin )];

    fifo->Begin = FifoNext( fifo, fifo->Begin );
    return data;
}

// Comprueba si la FIFO esta vacia
bool IsFifoEmpty( Fifo_t *fifo )
{
    return ( fifo->Begin == fifo->End );
}

// Comprueba si la FIFO esta llena
bool IsFifoFull( Fifo_t *fifo )
{
    return ( FifoNext( fifo, fifo->End ) == fifo->Begin );
}

// Lee el indice de la siguiente posicion en FIFO (es circular)
static uint16_t FifoNext( Fifo_t *fifo, uint16_t index )
{
    return ( index + 1 ) % fifo->Size;
}

// Rutina de Servicio a Interrupcion (ISR) de la UART
void EUSCIA0_IRQHandler(void)
{
    uint8_t data;

    // Lee el estado de la interrupcion generada por la UART
    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A0_BASE);

    // Reset del flag de interrupcion
    MAP_UART_clearInterruptFlag(EUSCI_A0_BASE, status);

    // Comprueba si es la interrupcion de recepcion
    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        // Lee el byte recibido de la UART
        data = MAP_UART_receiveData(EUSCI_A0_BASE);

        // Comprueba si la FIFO de recepcion esta llena
        if( IsFifoFull( &FifoRx ) == false )
        {
            // Escribe el byte recibido en la FIFO
            FifoPush( &FifoRx, data );
        }
        // Llama al call-back de recepcion
        Uart_rxCharCb();
    }

    // Comprueba si es la interrupcion de transmision
    if(status & EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG)
    {
        // Comprueba si la FIFO de transmision esta vacia
        if( IsFifoEmpty( &FifoTx ) == false )
        {
            // Lee un byte de la FIFO
            data = FifoPop( &FifoTx );

            //  Transmite el byte a traves de la UART
            MAP_UART_transmitData(EUSCI_A0_BASE, data);
        }
        else
        {
            MAP_UART_transmitData(EUSCI_A0_BASE, '\0');

            // Inhabilita la interrupcion de transmision de la UART
            MAP_UART_disableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT);
        }
    }
}




