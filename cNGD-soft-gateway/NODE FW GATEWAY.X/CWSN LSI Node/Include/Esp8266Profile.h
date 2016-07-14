/*******************************************************************************
* FileName:	Esp8266Profile.h
* Dependencies: See INCLUDES section
* Author:  David Torres Retamosa - B105 - UPM
********************************************************************************
* File Description:
* Creates the macros and definitions to use the UART3. 
*******************************************************************************/

#include "Compiler.h"
#include "GenericTypeDefs.h"
#include "HardwareConfig.h"
////////////////////////////////////////////////////////////////////////////////
// Define the functions you can use with the dedicated UART for ESP8266
////////////////////////////////////////////////////////////////////////////////

/* USE UART MACROS AND DEFINITIONS ******************************************/
#if defined USE_UART1
    #define UARTBRG                U1BRG
    #define UARTMODE               U1MODE
    #define UARTSTA                U1STA
    #define UARTBusyUART()          !U1STAbits.TRMT
    #define UARTTxNotRdyUART()      !U1STAbits.UTXBF
    #define UARTDataRdyUART()       U1STAbits.URXDA
    #define UARTReadUART()          (unsigned int) UARTGetDataByte(UART1)
    #define UARTWriteUART(a)        UARTSendDataByte(UART1, a)
    #define UARTgetcUART()          UARTGetDataByte(UART1);
#elif defined USE_UART2
    #define UARTBRG                U2BRG
    #define UARTMODE               U2MODE
    #define UARTSTA                U2STA
    #define UARTBusyUART()          !U2STAbits.TRMT
    #define UARTTxNotRdyUART()      !U2STAbits.UTXBF
    #define UARTDataRdyUART()       U2STAbits.URXDA
    #define UARTReadUART()          (unsigned int) UARTGetDataByte(UART2)
    #define UARTWriteUART(a)        UARTSendDataByte(UART2, a)
    #define UARTgetcUART()          UARTGetDataByte(UART2);
#elif defined USE_UART3
    #define UARTBRG                U3BRG
    #define UARTMODE               U3MODE
    #define UARTSTA                U3STA
    #define UARTBusyUART()          !U3STAbits.TRMT
    #define UARTTxNotRdyUART()      !U3STAbits.UTXBF
    #define UARTDataRdyUART()       U3STAbits.URXDA
    #define UARTReadUART()          (unsigned int) UARTGetDataByte(UART3)
    #define UARTWriteUART(a)        UARTSendDataByte(UART3, a)
    #define UARTgetcUART()          UARTGetDataByte(UART3);
#elif defined USE_UART4
    #define UARTBRG                U4BRG
    #define UARTMODE               U4MODE
    #define UARTSTA                U4STA
    #define UARTBusyUART()          !U4STAbits.TRMT
    #define UARTTxNotRdyUART()      !U4STAbits.UTXBF
    #define UARTDataRdyUART()       U4STAbits.URXDA
    #define UARTReadUART()          (unsigned int) UARTGetDataByte(UART4)
    #define UARTWriteUART(a)        UARTSendDataByte(UART4, a)
    #define UARTgetcUART()          UARTGetDataByte(UART4);
#elif defined USE_UART5
    #define UARTBRG                U5BRG
    #define UARTMODE               U5MODE
    #define UARTSTA                U5STA
    #define UARTBusyUART()          !U5STAbits.TRMT
    #define UARTTxNotRdyUART()      !U5STAbits.UTXBF
    #define UARTDataRdyUART()       U5STAbits.URXDA
    #define UARTReadUART()          (unsigned int) UARTGetDataByte(UART5)
    #define UARTWriteUART(a)        UARTSendDataByte(UART5, a)
    #define UARTgetcUART()          UARTGetDataByte(UART5);
#elif defined USE_UART6
    #define UARTBRG                U6BRG
    #define UARTMODE               U6MODE
    #define UARTSTA                U6STA
    #define UARTBusyUART()          !U6STAbits.TRMT
    #define UARTTxNotRdyUART()      !U6STAbits.UTXBF
    #define UARTDataRdyUART()       U6STAbits.URXDA
    #define UARTReadUART()          (unsigned int) UARTGetDataByte(UART6)
    #define UARTWriteUART(a)        UARTSendDataByte(UART6, a)
    #define UARTgetcUART()          UARTGetDataByte(UART6);
#endif