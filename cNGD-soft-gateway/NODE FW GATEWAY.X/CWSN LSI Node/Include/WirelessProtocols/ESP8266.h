/*******************************************************************************
* FileName:	ESP8266.h
* Dependences: See INCLUDES section
* Author:  David Torres Retamosa - B105 - UPM
********************************************************************************
* File Description:
* This header file allows usage of the ESP8266 functions located in ESP8266.c
*******************************************************************************/

#ifndef  _ESP8266_H_
#define  _ESP8266_H_

/********************************* HEADERS ************************************/
#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "ConfigApp.h"
#include "HardwareProfile.h"
#include "Esp8266Profile.h"
#include "../NodeHAL.h"

//#include "USB/usb_console.h"

/******************************* DEFINITIONS **********************************/
#define BAUD_RATE 115200    //Typ. 9600, 19200, 38400, 57600, 115200, 230400...
#define USB_BAUD_RATE 19200 //Fix

/* LSI-CWSN Node: UART MODE configuration depending on UART BAUD RATE **********
 * Default Node Configuration: SYSCLK = 80 MHz, PBCLK = 20 MHz (SYSCLK / PBDIV).
 * Default UART Configuration: UART enabled, 8 bits data, no parity, 1 stop bit.
 *
 * Depending on chosen Baud Rate, BRGH (High Baud Rate Enable Bit) is set:
 *      - If Baud Rate is 115200 or higher => High Speed Mode, UBRGH_DIV = 4
 *      - Otherwise => Normal Speed Mode, UBRGH_DIV = 16
 *   UBRG register will be configured to get the desired speed:
 *      => UBRG = ( (SYSCLK / PBDIV) / (UBRGH_DIV * BAUD_RATE) ) - 1
 ******************************************************************************/
    #if (BAUD_RATE >= 115200)
        #define UMODE_CONF  0x8008
        #define UBRGH_DIV   4
        // UART Enabled, High Speed BRG, 8 bits data, no parity, 1 stop bit.
    #else
        #define UMODE_CONF  0x8000
        #define UBRGH_DIV   16
        // UART Enabled, Normal Speed BRG, 8 bits data, no parity, 1 stop bit.
    #endif

/**************************** FUNCTION PROTOTYPES *****************************/
#if defined(__dsPIC30F__) || defined(__dsPIC33F__) || defined(__PIC24F__) || \
    defined(__PIC24FK__)  || defined(__PIC24H__)   || defined(__PIC32MX__)

    #define TEST_NODE

    #if defined(ENABLE_ESP8266)
        void ESP8266Init(void);
        void ESP8266Put(BYTE c);
        BYTE ESP8266Get();
        void ESP8266PrintChar(BYTE);
        void ESP8266PutROMString(ROM char* str);
        void ESP8266PrintDec(BYTE);
        
        int ESP8266Decode(char* receivedData, int charcounter);
        UINT8 ESP8266instructionCode(char* insCode);
        UINT8 ESP8266numberOfNodes (char numnodes);
        UINT8 ESP8266involvedNodes(char* invNodes);
        UINT8 ESP8266sameForAll(char sameAll);
        void ESP8266involvedReg(char* invReg);
        void ESP8266value(char* value);
        
        UINT8 caseRead(char* receivedData, int charcounter);
        UINT8 caseWrite(char* receivedData, int charcounter);
        UINT8 caseMonitor(char* receivedData, int charcounter);
        UINT8 caseNotification(char* receivedData, int charcounter);
        UINT8 caseReset(char* receivedData, int charcounter);
        UINT8 caseTraces(char* receivedData, int charcounter);
        UINT8 caseBootloader(char* receivedData, int charcounter);
        
        UINT8 decodeReadLikeFunction(char* receivedData, int charcounter);
        UINT8 decodeReadSomeLikeFunction(char* receivedData, int charcounter);
        UINT8 decodeWriteLikeFunction(char* receivedData, int charcounter);
        UINT8 decodeWriteSomeLikeFunction(char* receivedData, int charcounter);
        
        UINT8 ESP8266SendCommands(BYTE lessNoisyChannel, radioInterface rx);
        
        UINT8 asciiToHex(char ch);
        void ESP8266ClearData(void);
        void ESP8266ClearDataToSend(void);
        
        void SendViaESP8266(char* x);
        
        //#if defined DEBUG_USB

            //#define ESP8266IsGetReady()   (IFS1bits.U3RXIF)//XXX-Willy.
            //#define ESP8266IsPutReady()   (U3STAbits.TRMT)//XXX-Willy.

        //#else
        
            #define ESP8266IsPutReady()     (U3STAbits.TRMT)
            #define ESP8266IsGetReady()     (IFS1bits.U3RXIF)

        //#endif
    #else
        #define ESP8266Init()
        #define ESP8266IsPutReady() 1
        #define ESP8266Put(c)
        #define ESP8266PutString(s)
        #define ESP8266PutROMString(str)

        #define ESP8266IsGetReady() 1
        #define ESP8266Get(int iteration)        'a'
        #define ESP8266GetString(buffer, bufferLen) 0
        #define ESP8266PrintChar(a)
        #define ESP8266PrintDec(a)
    #endif
    #if defined TEST_NODE
        void testButtonsAndLeds(void);        
    #endif

#else
    #error Unknown processor.  See Compiler.h
#endif

    #define SendViaESP8266(x) ESP8266PutROMString((ROM char*)x)
        
#endif

