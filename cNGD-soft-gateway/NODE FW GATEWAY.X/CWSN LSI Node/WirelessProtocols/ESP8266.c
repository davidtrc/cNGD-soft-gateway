/*******************************************************************************
* FileName:	ESP8266.c
* Dependencies: See INCLUDES section
* Author:  David Torres Retamosa - B105 - UPM
********************************************************************************
* File Description:
* This file configures and provides the function for using the UART to transmit 
* data over ESP8266.
*******************************************************************************/

/********************************** HEADERS ***********************************/
#include "WirelessProtocols/Console.h" //Included for DEBUG_USB functions
#include "WirelessProtocols/ESP8266.h"
#include "WirelessProtocols/ConfigApp.h"
#include "Compiler.h"
#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "Esp8266Profile.h"
#include "NodeHAL.h"

#include "USB/usb_config.h"
#include "./USB/usb.h"
#include "./USB/usb_function_cdc.h"
#include "USB/usb_device.h"

#define READ 0x1
#define READ_SOME 0x2
#define WRITE 0x3
#define WRITE_SOME 0x4
#define MONITOR 0x5
#define MONITOR_SOME 0x6
#define NO_MONITOR 0x7
#define NO_MONITOR_SOME 0x8
#define NOTIFY_CHANGE 0x9
#define NOTIFY_SOME 0xA
#define NO_NOTIFY_CHANGE 0xB
#define NO_NOTIFY_CHANGE_SOME 0xC
#define RESET_NODE 0xD
#define GET_TRACES 0xE
#define BOOTLOADER 0xF

#define PACKET_SIZE 1024
#define ADDRESS_SIZE 8

#if defined(ENABLE_ESP8266)
#if defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24FK__) || \
    defined(__PIC24H__) || defined(__PIC32MX__)

/******************************* VARIABLES ************************************/
ROM unsigned char CharracterArrayESP8266[]={'0','1','2','3','4','5','6','7','8','9','A',
                                    'B','C','D','E','F'};
unsigned char TempCharracterArrayESP8266[] = {'a','a','a'};
static unsigned char instructionCode[] = {'0', '0'};
static unsigned char numberOfNodes;
static unsigned char involvedNodes[112]; //16*7
static unsigned char involvedNodesSplit[7][16];
static unsigned char sameForAll;
static unsigned char involvedReg[56]; //8*7
static unsigned char involvedRegSplit[7][8];
static unsigned char value[56];
static unsigned char valueSplit[7][8];

unsigned char packetToSend[PACKET_SIZE];
unsigned char packetToSendTemp[1026];
BYTE addressToSend[ADDRESS_SIZE];

UINT8 instructionCodeToInt = 0;
UINT8 numberOfNodesToInt = 0;
UINT8 sameForAllToInt = 0;

/*******************************************************************************
* Function:         void ESP8266Init(void)
* PreCondition:     none
* Input:            none
* Output:	    none
* Side Effects:	    UART selected for debugging is configured
* Overview:         This function will configure the UART with the options:
*                   8 bits data, 1 stop bit, no flowcontrol mode
* Note:		    None
*******************************************************************************/
void ESP8266Init(void){
#if defined __32MX795F512H__ || defined __32MX795F512L__ || defined __32MX675F256L__
        #if defined TEST_NODE
            LED1_TRIS = OUTPUT_PIN;
            LED2_TRIS = OUTPUT_PIN;
            LED3_TRIS = OUTPUT_PIN;
            LED1 = 0;
            LED2 = 0;
            LED3 = 0;   
        #endif

        #if defined USE_UART1
            U1BRG = ((CLOCK_FREQ/(1<<mOSCGetPBDIV()))/(UBRGH_DIV*BAUD_RATE))-1;
            U1STA = 0;
            U1MODE = UMODE_CONF;
            U1STA = (1 << 12)|UART_TX_ENABLE;   //0x1400    TX/RX EN
            HEADER_01_TRIS = OUTPUT_PIN;
            HEADER_17_TRIS = INPUT_PIN;
            AD1PCFG = AD1PCFG | 0x4000; //Enable the HEADER_01 as a digital pin
            HEADER_01 = 0;
            HEADER_17 = 0;
        #elif defined USE_UART2
            U2BRG = ((CLOCK_FREQ/(1<<mOSCGetPBDIV()))/(UBRGH_DIV*BAUD_RATE))-1;
            U2STA = 0;
            U2MODE = UMODE_CONF;
            U2STA = (1 << 12)|UART_TX_ENABLE;   //0x1400    TX/RX EN
            HEADER_01_TRIS = OUTPUT_PIN;
            HEADER_17_TRIS = INPUT_PIN;
            AD1PCFG = AD1PCFG | 0x4000; //Enable the HEADER_01 as a digital pin
            HEADER_01 = 0;
            HEADER_17 = 0;
        #elif defined USE_UART3
            U3BRG = ((CLOCK_FREQ/(1<<mOSCGetPBDIV()))/(UBRGH_DIV*BAUD_RATE))-1;
            U3STA = 0;
            U3MODE = UMODE_CONF;
            U3STA = (1 << 12)|UART_TX_ENABLE;   //0x1400    TX/RX EN
            HEADER_01_TRIS = OUTPUT_PIN;
            HEADER_17_TRIS = INPUT_PIN;
            AD1PCFG = AD1PCFG | 0x4000; //Enable the HEADER_01 as a digital pin
            HEADER_01 = 0;
            HEADER_17 = 0;
        #elif defined USE_UART4
            U4BRG = ((CLOCK_FREQ/(1<<mOSCGetPBDIV()))/(UBRGH_DIV*BAUD_RATE))-1;
            U4STA = 0;
            U4MODE = UMODE_CONF;
            U4STA = (1 << 12)|UART_TX_ENABLE;   //0x1400    TX/RX EN
            HEADER_01_TRIS = OUTPUT_PIN;
            HEADER_17_TRIS = INPUT_PIN;
            AD1PCFG = AD1PCFG | 0x4000; //Enable the HEADER_01 as a digital pin
            HEADER_01 = 0;
            HEADER_17 = 0;
        #elif defined USE_UART5
            U5BRG = ((CLOCK_FREQ/(1<<mOSCGetPBDIV()))/(UBRGH_DIV*BAUD_RATE))-1;
            U5STA = 0;
            U5MODE = UMODE_CONF;
            U5STA = (1 << 12)|UART_TX_ENABLE;   //0x1400    TX/RX EN
            HEADER_01_TRIS = OUTPUT_PIN;
            HEADER_17_TRIS = INPUT_PIN;
            AD1PCFG = AD1PCFG | 0x4000; //Enable the HEADER_01 as a digital pin
            HEADER_01 = 0;
            HEADER_17 = 0;
        #elif defined USE_UART6
            U6BRG = ((CLOCK_FREQ/(1<<mOSCGetPBDIV()))/(UBRGH_DIV*BAUD_RATE))-1;
            U6STA = 0;
            U6MODE = UMODE_CONF;
            U6STA = (1 << 12)|UART_TX_ENABLE;   //0x1400    TX/RX EN
            HEADER_01_TRIS = OUTPUT_PIN;
            HEADER_17_TRIS = INPUT_PIN;
            AD1PCFG = AD1PCFG | 0x4000; //Enable the HEADER_01 as a digital pin
            HEADER_01 = 0;
            HEADER_17 = 0;
        #endif
    #else    
        #error Microcontroller not supported.
    #endif
}

/*******************************************************************************
* Function:         void ESP8266Put(BYTE c)
* PreCondition:     none
* Input:	    c - character to be printed
* Output:           none
* Side Effects:	    c is printed to the ESP8266
* Overview:         This function will print the inputed character
* Note:             Do not power down the microcontroller until the transmission
*                   is complete or the last transmission of the string can be
*                   corrupted.
*******************************************************************************/
void ESP8266Put(BYTE c){
    #if defined USE_UART1
        while(U1STAbits.TRMT == 0);
        U1TXREG = c;
    #elif defined USE_UART2
        while(U2STAbits.TRMT == 0);
        U2TXREG = c;
    #elif defined USE_UART3
        while(U3STAbits.TRMT == 0);
        U3TXREG = c;
    #elif defined USE_UART4
        while(U4STAbits.TRMT == 0);
        U4TXREG = c;
    #elif defined USE_UART5
        while(U5STAbits.TRMT == 0);
        U5TXREG = c;
    #elif defined USE_UART6
        while(U6STAbits.TRMT == 0);
        U6TXREG = c;
    #endif
}

/*******************************************************************************
* Function:         BYTE ESP8266Get(void)
* PreCondition:     none
* Input:	    none
* Output:	    one byte received by UART
* Side Effects:	    none
* Overview:	    This function will receive one byte from UART
* Note:             Do not power down the microcontroller until the transmission
*                   is complete or the last transmission of the string can be
*                   corrupted.  
*******************************************************************************/
BYTE ESP8266Get(){
    char Temp;
    #if defined USE_UART1
        while(IFS0bits.U1RXIF == 0){
            if(HEADER_17 == 0){
                return;
            }
        };
        Temp = U1RXREG;
        IFS0bits.U1RXIF = 0;
    #elif defined USE_UART2
        while(IFS1bits.U2RXIF == 0){
            if(HEADER_17 == 0){
                return;
            }
        };
        Temp = U2RXREG;
        IFS1bits.U2RXIF = 0;
    #elif defined USE_UART3
        while(IFS1bits.U3RXIF == 0){
            if(HEADER_17 == 0){
                return;
            }
        };
        Temp = U3RXREG;
        IFS1bits.U3RXIF = 0;
    #elif defined USE_UART4
        while(IFS2bits.U4RXIF == 0){
            if(HEADER_17 == 0){
                return;
            }
        };
        Temp = U4RXREG;
        IFS2bits.U4RXIF = 0;
    #elif defined USE_UART5
        while(IFS2bits.U5RXIF == 0){
            if(HEADER_17 == 0){
                return;
            }
        };
        Temp = U5RXREG;
        IFS2bits.U5RXIF = 0;
    #elif defined USE_UART6
        while(IFS2bits.U6RXIF == 0){
            if(HEADER_17 == 0){
                return;
            }
        };
        Temp = U6RXREG;
        IFS2bits.U6RXIF = 0;
     #endif
    return Temp;
}

/*******************************************************************************
* Function:         void ESP8266PrintChar(BYTE toPrint)
* PreCondition:     none
* Input:	    toPrint - character to be printed
* Output:           none
* Side Effects:	    toPrint is printed to the ESP8266
* Overview:         This function will print the inputed BYTE to the ESP8266 in
*                   hexidecimal form
* Note:             Do not power down the microcontroller until the transmission
*                   is complete or the last transmission of the string can be
*                   corrupted.
*******************************************************************************/
void ESP8266PrintChar(BYTE toPrint){
    BYTE PRINT_VAR;
    PRINT_VAR = toPrint;
    toPrint = (toPrint>>4)&0x0F;

    ESP8266Put(CharracterArrayESP8266[toPrint]);

    toPrint = (PRINT_VAR)&0x0F;

    ESP8266Put(CharracterArrayESP8266[toPrint]);

}

/*******************************************************************************
* Function:         void ESP8266PutROMString(ROM char* str)
* PreCondition:     none
* Input:	    str - ROM string that needs to be printed
* Output:           none
* Side Effects:	    str is printed to the ESP8266
* Overview:         This function will print the inputed ROM string
* Note:             Do not power down the microcontroller until the transmission
*                   is complete or the last transmission of the string can be
*                   corrupted.
*******************************************************************************/
void ESP8266PutROMString(ROM char* str){
    
    BYTE c;
    while((c = *str++)){
        ESP8266Put(c);
    }
}

/*******************************************************************************
* Function:         void ESP8266PrintDec(BYTE toPrint)
* PreCondition:     none
* Input:	    toPrint - character to be printed. Range is 0-99
* Output:           none
* Side Effects:	    toPrint is printed to the ESP8266 in decimal
* Overview:         This function will print the inputed BYTE to the ESP8266 in
*                   decimal form
* Note:             Do not power down the microcontroller until the transmission
*                   is complete or the last transmission of the string can be
*                   corrupted.
*******************************************************************************/
void ESP8266PrintDec(BYTE toPrint){
    BYTE a = toPrint/10;
    BYTE b = toPrint%10;
    char aa;
    if(toPrint>9){
        aa = CharracterArrayESP8266[a];
        }else{
        aa = '0';
        }
    char bb = CharracterArrayESP8266[b];
    TempCharracterArrayESP8266[0]=aa;
    TempCharracterArrayESP8266[1]=bb;
    TempCharracterArrayESP8266[2]='\0';
    ESP8266PutROMString(&TempCharracterArrayESP8266[0]);
}
 
/*******************************************************************************
* Function:         int ESP8266Decode(char* receivedData, int charcounter)
* Input:            Received Data and the length of that char*
* Output:           an int. 1 if the command exists, 0 if doesn't
* Overview:         This function gets the instruction code from the received
*                   data. Place here any new instruction type. After, calls an
*                   another, specific and depending of the instruction code.
*******************************************************************************/
int ESP8266Decode(char* receivedData, int charcounter){
    UINT8 result;
    Printf(receivedData);   
    Printf("\r");
    
    //DECODE INSTRUCTION CODE
    instructionCode[0] = receivedData[0];
    instructionCode[1] = receivedData[1];
    instructionCodeToInt = ESP8266instructionCode(instructionCode);
    if(instructionCodeToInt == 0){
        return 0;
    }
    
    Printf("Instruction Code = ");
    Printf(instructionCode);
    Printf("\r");
    
    //DECODE NUMBER OF NODES
    numberOfNodes = receivedData[2];
    numberOfNodesToInt = ESP8266numberOfNodes(numberOfNodes);
    
    switch(instructionCodeToInt){
        case(READ):
            Printf("READ\r");
            result = caseRead(receivedData, charcounter);
            break;
        case(READ_SOME):
            Printf("READ SOME NODES\r");
            result = caseRead(receivedData, charcounter);
            break;
        case(WRITE):
            Printf("WRITE\r");
            result = caseWrite(receivedData, charcounter);
            break;
        case(WRITE_SOME):
            Printf("WRITE SOME NODES\r");
            result = caseWrite(receivedData, charcounter);
            break;
        case(MONITOR):
            Printf("MONITOR\r");
            result = caseMonitor(receivedData, charcounter);
            break;
        case(MONITOR_SOME):
            Printf("MONITOR SOME NODES\r");
            result = caseMonitor(receivedData, charcounter);
            break;
        case(NO_MONITOR):
            Printf("NO MONITOR\r");
            result = caseMonitor(receivedData, charcounter);
            break;
        case(NO_MONITOR_SOME):
            Printf("NO MONITOR SOME NODES\r");
            result = caseMonitor(receivedData, charcounter);
            break;
        case(NOTIFY_CHANGE):
            Printf("NOTIFY CHANGE\r");
            result = caseNotification(receivedData, charcounter);
            break;
        case(NOTIFY_SOME):
            Printf("NOTIFY CHANGE IN SOME NODES\r");
            result = caseNotification(receivedData, charcounter);
            break;
        case(NO_NOTIFY_CHANGE):
            Printf("NO NOTIFY CHANGE\r");
            result = caseNotification(receivedData, charcounter);
            break;
        case(NO_NOTIFY_CHANGE_SOME):
            Printf("NO NOTIFY CHANGE IN SOME NODES\r");
            result = caseNotification(receivedData, charcounter);
            break;
        case(RESET_NODE):
            Printf("RESET NODE\r");
            result = caseReset(receivedData, charcounter);
            break;
        case(GET_TRACES):
            Printf("GET TRACES\r");
            result = caseTraces(receivedData, charcounter);
            break;
        case(BOOTLOADER):
            Printf("BOOTLOADER\r");
            result = caseBootloader(receivedData, charcounter);
            break;
    }
    if(result == 0){
        return result;
    }
    return 1;
}

/*******************************************************************************
* Function:         UINT8 ESP8266instructionCode(char* insCode)
* Input:            char* with two first chars of the received chain
* Output:           int with instruction. 0 if wrong
* Overview:         Decode the characters passed (the instruction code) 
*                   and return a int, based on a DEFINE, with the instruction.
*                   The instructions are subdivided in groups, p.ex, instructions
*                   starting with 1 are read instruction, with 2, write, etc...
*******************************************************************************/
UINT8 ESP8266instructionCode(char* insCode){
    if(insCode[0] == '1'){ //READ FUNCTIONS
        if(insCode[1] == '0'){
            return READ;
        } else if (insCode[1] == '1'){
           return READ_SOME;
        }
        //PLACE HERE REST OF SPECIFIC READ FUNCTIONS
    } else if (insCode[0] == '2') {
        if(insCode[1] == '0'){
           return WRITE;
        } else if (insCode[1] == '1'){
           return WRITE_SOME;
        }
        //PLACE HERE REST OF SPECIFIC WRITE FUNCTIONS
    } else if (insCode[0] == '3') {
        if(insCode[1] == '0'){
           return MONITOR;
        } else if (insCode[1] == '1'){
           return MONITOR_SOME;
        } else if (insCode[1] == '2'){
           return NO_MONITOR;
        } else if (insCode[1] == '3'){
           return NO_MONITOR_SOME;
        }
        //PLACE HERE REST OF SPECIFIC MONITOR FUNCTIONS
    } else if (insCode[0] == '4') {
        if(insCode[1] == '0'){
           return NOTIFY_CHANGE;
        } else if (insCode[1] == '1'){
           return NOTIFY_SOME;
        } else if (insCode[1] == '2'){
           return NO_NOTIFY_CHANGE;
        } else if (insCode[1] == '3'){
           return NO_NOTIFY_CHANGE_SOME;
        }
        //PLACE HERE REST OF SPECIFIC NOTIFICATION FUNCTIONS
    } else if (insCode[0] == '5') {
        if(insCode[1] == '0'){
           return RESET_NODE;
        }
        //PLACE HERE REST OF SPECIFIC RESET FUNCTIONS
    } else if (insCode[0] == '6') {
        if(insCode[1] == '0'){
           return GET_TRACES;
        }
        //PLACE HERE REST OF SPECIFIC TRACES RECUPERATION FUNCTIONS
    } else if (insCode[0] == '7') {
        if(insCode[1] == '0'){
           return BOOTLOADER;
        }
        //PLACE HERE REST OF SPECIFIC BOOTLOADER FUNCTIONS
    }
    Printf("Incorrect instruction Code");
    return 0;                           
}

/*******************************************************************************
* Function:         UINT8 ESP8266numberOfNodes(char numnodes)
* Input:            third char of the received chain
* Output:           int with number of nodes or 99 if broadcast. 0 if wrong
* Overview:         Decode the char passed (the number of nodes) 
*                   and return the ASCII value as an int, or 99 if the packet is
*                   broadcast or 0 if the char is not was expected.
*******************************************************************************/
UINT8 ESP8266numberOfNodes (char numnodes) {
    int temp = 0;
    
    if(numnodes == 'F'){
        return 0xF;
    }
    
    if(numnodes == '1'){
        temp += 1;
    } else if(numnodes == '2'){
        temp += 2;
    }
    else if(numnodes == '3'){
        temp += 3;
    }
    else if(numnodes == '4'){
        temp += 4;
    }
    else if(numnodes == '5'){
        temp += 5;
    }
    else if(numnodes == '6'){
        temp += 6;
    }
    else if(numnodes == '7'){
        temp += 7;
    };
    
    if(temp > 7 || temp == 0){
        return 0;
    }
    return temp;
}

/*******************************************************************************
* Function:         UINT8 ESP8266involvedNodes(char* invNodes)
* Input:            char* with all the nodes passed in the packet
* Output:           none
* Overview:         Split the char* passed in to the involved nodes (formed by 16
*                   chars), and save them in involvedNodesSplit.
*******************************************************************************/
UINT8 ESP8266involvedNodes(char* invNodes){
    UINT16 i;
    UINT8 j;
    UINT8 k = 0;
    UINT8 m = 15;
    char temp1;
    char temp2;
    
    for(i = 0; i<numberOfNodesToInt; i++){
        for(j = 0; j<8; j++){
            temp1 = invNodes[k];
            k++;
            temp2 = invNodes[k];
            k++;
            involvedNodesSplit[i][m] = temp2;
            m--;
            involvedNodesSplit[i][m] = temp1;
            m--;
            ConsolePut(temp2);
            ConsolePut(temp1);
        }
        Printf("\r");
    }
    
    /*for(i = 0; i<numberOfNodesToInt; i++){
        for(j = 0; j<16; j++){
            involvedNodesSplit[i][j] = invNodes[k];
            ConsolePut(invNodes[k]);
            k++;
        }
        Printf("\r");
    }*/
    return 1;
}

/*******************************************************************************
* Function:         UINT8 ESP8266sameForAll(char sameAll)
* Input:            char with same for All byte
* Output:           int equals to 10 if each node will operate in different register,
*                   1 if all will operate the same or the number of register
*                   will be R or W (only in read some or write some functions)
* Overview:         explained in output
*******************************************************************************/
UINT8 ESP8266sameForAll(char sameAll){
    if (sameAll == '0'){
        return 10;
    } else if (sameAll == '1'){
        return 1;
    }  else if (sameAll == '2'){
        return 2;
    } else if (sameAll == '3'){
        return 3;
    } else if (sameAll == '4'){
        return 4;
    } else if (sameAll == '5'){
        return 5;
    } else if (sameAll == '6'){
        return 6;
    } else if (sameAll == '7'){
        return 7;
    } else if (sameAll == '8'){
        return 8;
    } else if (sameAll == '9'){
        return 9;
    }
    return 0;
}

/*******************************************************************************
* Function:         void ESP8266involvedReg(char* invReg)
* Input:            char* with all registers passed in the packet
* Output:           none
* Overview:         Split the char* passed in to the involved nodes (formed by 8
*                   chars), and save them in involvedRegSplit.
*******************************************************************************/
void ESP8266involvedReg(char* invReg){
    UINT8 i;
    UINT8 j;
    UINT8 k = 0;
    UINT8 multiplier = 1;
    
    if(numberOfNodesToInt == 0xF){
        for(i = 0; i<sameForAllToInt; i++){
            for(j = 0; j<8; j++){
                involvedRegSplit[i][j] = invReg[k];
                ConsolePut(invReg[k]);
                k++;
            }
            Printf("\r");
        }
        return;
    }
    
    if((instructionCodeToInt == READ_SOME) || (instructionCodeToInt == WRITE_SOME)){
        multiplier = sameForAllToInt;
    }
    
    else if( ((instructionCodeToInt == READ) || (instructionCodeToInt == WRITE) || (instructionCodeToInt == MONITOR) || (instructionCodeToInt == NO_MONITOR) || 
            (instructionCodeToInt == NOTIFY_CHANGE) || (instructionCodeToInt == NO_NOTIFY_CHANGE) || (instructionCodeToInt == RESET_NODE)) && (sameForAllToInt == 1)){
       
        for(i = 0; i<multiplier; i++){
            for(j = 0; j<8; j++){
                involvedRegSplit[i][j] = invReg[k];
                ConsolePut(invReg[k]);
                k++;
            }
        Printf("\r");
        } 
        return;
    }
    
    for(i = 0; i<numberOfNodesToInt*multiplier; i++){
        for(j = 0; j<8; j++){
            involvedRegSplit[i][j] = invReg[k];
            ConsolePut(invReg[k]);
            k++;
        }
        Printf("\r");
    }
}

/*******************************************************************************
* Function:         void ESP8266value(char* value)
* Input:            char* with values of the registers to be R or W
* Output:           none
* Overview:         Split the char* passed in to the values (formed by 8
*                   chars), and save them in involvedRegSplit.
*******************************************************************************/
void ESP8266value(char* value){
    UINT8 i;
    UINT8 j;
    UINT8 k = 0;
    UINT8 multiplier = 1;
    
    if((instructionCodeToInt == READ_SOME) || (instructionCodeToInt == WRITE_SOME)){
        multiplier = sameForAllToInt;
    }
    
    for(i = 0; i<numberOfNodesToInt*multiplier; i++){
        for(j = 0; j<8; j++){
            valueSplit[i][j] = value[k];
            ConsolePut(value[k]);
            k++;
        }
        Printf("\r");
    }
}

/*******************************************************************************
* Function:         UINT8 caseRead(char* receivedData, int charcounter)
* Input:            char* with the entire packet. An int with the packet size
* Output:           1 if the command is correct formated, 0 if not
* Overview:         Process packets with every type of read instructions. Has 
*                   two types. Read (some nodes, one 
*                   register per node) and read some (one node, some registers).
*******************************************************************************/
UINT8 caseRead(char* receivedData, int charcounter){
    UINT8 var;
    
    switch(instructionCodeToInt){
        case(READ):    
            var = decodeReadLikeFunction(receivedData, charcounter);
            break;
        case(READ_SOME):
            var = decodeReadSomeLikeFunction(receivedData, charcounter);
            break;
        //PLACE HERE REST OF SPECIFIC READ FUNCTIONS
            
        default:
            return 0;
    }
    
    return var;
}

/*******************************************************************************
* Function:         UINT8 caseWrite(char* receivedData, int charcounter)
* Input:            char* with the entire packet. An int with the packet size
* Output:           1 if the command is correct formated, 0 if not
* Overview:         Process packets with every type of Write instructions. Has 
*                   two types. Write (some nodes, one 
*                   register per node) and Write some (one node, some registers).
*******************************************************************************/
UINT8 caseWrite(char* receivedData, int charcounter){
    UINT8 var;
    
    switch(instructionCodeToInt){
        case(WRITE):    
            var = decodeWriteLikeFunction(receivedData, charcounter);
            break;
    
        case(WRITE_SOME):
            var = decodeWriteSomeLikeFunction(receivedData, charcounter);
            break;
        
        //PLACE HERE REST OF SPECIFIC WRITE FUNCTIONS
            
        default:
            return 0;
    }
    return var;
}

/*******************************************************************************
* Function:         UINT8 caseMonitor(char* receivedData, int charcounter)
* Input:            char* with the entire packet. An int with the packet size
* Output:           1 if the command is correct formated, 0 if not
* Overview:         Process packets with every type of monitor instructions. Has 
*                   four types. Monitor (some nodes, one register per node), 
*                   monitor some (one node, some registers), no monitor (unmake
*                   monitor operation) and no monitor some (unmake monitor 
*                   operation for some nodes).
*******************************************************************************/
UINT8 caseMonitor(char* receivedData, int charcounter){
    UINT8 var;
    
    switch(instructionCodeToInt){
        case(MONITOR):    
            var = decodeWriteLikeFunction(receivedData, charcounter);
            break;
    
        case(MONITOR_SOME):
            var = decodeWriteSomeLikeFunction(receivedData, charcounter);
            break;
            
        case(NO_MONITOR):
            var = decodeReadLikeFunction(receivedData, charcounter);
            break;
            
        case(NO_MONITOR_SOME):
            var = decodeReadSomeLikeFunction(receivedData, charcounter);
            break;
        
        //PLACE HERE REST OF SPECIFIC MONITOR FUNCTIONS
            
        default:
            return 0;
    }
    return var;
}

/*******************************************************************************
* Function:         UINT8 caseNotification(char* receivedData, int charcounter)
* Input:            char* with the entire packet. An int with the packet size
* Output:           1 if the command is correct formated, 0 if not
* Overview:         Process packets with every type of notification instructions. Has 
*                   four types. Notify change (some nodes, one register per node), 
*                   notify some some (one node, some registers), no notify change (unmake
*                   notification operation) and no notify change some (unmake notification 
*                   operation for some nodes).
*******************************************************************************/
UINT8 caseNotification(char* receivedData, int charcounter){
    UINT8 var;
    
    switch(instructionCodeToInt){
        case(NOTIFY_CHANGE):    
            var = decodeReadLikeFunction(receivedData, charcounter);
            break;
    
        case(NOTIFY_SOME):
            var = decodeReadSomeLikeFunction(receivedData, charcounter);
            break;
            
        case(NO_NOTIFY_CHANGE):
            var = decodeReadLikeFunction(receivedData, charcounter);
            break;
            
        case(NO_NOTIFY_CHANGE_SOME):
            var = decodeReadSomeLikeFunction(receivedData, charcounter);
            break;
        
        //PLACE HERE REST OF SPECIFIC NOTIFICATION FUNCTIONS
            
        default:
            return 0;
    }
    return var;
}

/*******************************************************************************
* Function:         UINT8 caseReset(char* receivedData, int charcounter)
* Input:            char* with the entire packet. An int with the packet size
* Output:           1 if the command is correct formated, 0 if not
* Overview:         Process packets with every type of reset instructions. Has 
*                   one type. Reset node (clean all the notifications and monito
*                   rizations of a node.
*******************************************************************************/
UINT8 caseReset(char* receivedData, int charcounter){
    UINT8 var;
    
    switch(instructionCodeToInt){
        case(RESET_NODE):    
            var = decodeReadLikeFunction(receivedData, charcounter);
            break;
        //PLACE HERE REST OF SPECIFIC RESET FUNCTIONS
            
        default:
            return 0;
    }
    return var;
}

/*******************************************************************************
* Function:         UINT8 caseTraces(char* receivedData, int charcounter)
* Input:            char* with the entire packet. An int with the packet size
* Output:           1 if the command is correct formated, 0 if not
* Overview:         Use the read some structure. Thats means,  number of nodes 
*                   = 1, same for all = number of registers to be used, value 
*                   field discarded. In the registers fields write what about to
*                   receive traces.
*******************************************************************************/
UINT8 caseTraces(char* receivedData, int charcounter){
    UINT8 var;
    
    switch(instructionCodeToInt){
        case(GET_TRACES):    
            var = decodeReadSomeLikeFunction(receivedData, charcounter);
            break;
        //PLACE HERE REST OF SPECIFIC TRACES FUNCTIONS
            
        default:
            return 0;
    }
    
    return var;
}

/*******************************************************************************
* Function:         UINT8 caseBootloader(char* receivedData, int charcounter)
* Input:            char* with the entire packet. An int with the packet size
* Output:           1 if the command is correct formated, 0 if not
* Overview:         Not implemented yet. Made for a possible implementation of 
*                   the gateway to load code in a node.
*******************************************************************************/
UINT8 caseBootloader(char* receivedData, int charcounter){
    UINT8 var;
    
    switch(instructionCodeToInt){
        case(BOOTLOADER):
            break;
        //PLACE HERE REST OF SPECIFIC BOOTLOADER FUNCTIONS
            
        default:
            return 0;
    }
    return var;
}

/*******************************************************************************
* Function:         UINT8 decodeReadLikeFunction(char* receivedData, int charcounter)
* Input:            Received Data and the length of that char*
* Output:           An int. 1 if the packet is correct formatted, 0 if doesn't
* Overview:         This function gets the entire packet and checks if all the
*                   fields are correct formatted. Only called when the packet
*                   has a READ TYPE structure, you can see the structure type of
*                   the packet in caseXXXXXXXX function. 
*******************************************************************************/
UINT8 decodeReadLikeFunction(char* receivedData, int charcounter){
    UINT16 i;
    
    if((numberOfNodesToInt == 0) || (numberOfNodesToInt == 0xF)){
        Printf("Incorrect node amount \r ");
        return 0;
    }
    Printf("Number of nodes = ");
    PrintDec(numberOfNodesToInt);
    Printf("\r");

    //Make this first and if they are correct, then get the nodes ID, registers and values
    //DECODE FIRST CONTROL BYTES
    for(i=((numberOfNodesToInt*16)+3); i<((numberOfNodesToInt*16)+19); i++){
        if(receivedData[i] != '0'){
            Printf("First control bytes check failed\r");
            return 0;
        }
    }
    Printf("First control bytes are correct\r");
    
    if(instructionCodeToInt == RESET_NODE){
        //DECODE INVOLVED NODES
        for(i=3; i<((numberOfNodesToInt*16)+3); i++){
            involvedNodes[i-3] = receivedData[i];
        }
        Printf("Involved nodes = ");
        if( ESP8266involvedNodes(involvedNodes) == 0){
            return 0;
        }
        
        Printf("Discarded other fields for being a reset petition\r");
        return 1;
        
    }

    //DECODE SAME REGISTER FOR ALL
    sameForAll = receivedData[((numberOfNodesToInt*16)+19)];
    sameForAllToInt = ESP8266sameForAll(sameForAll);
    if(sameForAllToInt == 0 || (sameForAllToInt != 1 && sameForAllToInt != 10) ){
        Printf("Unknow Same for all value");
        return 0;
    }
    if(sameForAllToInt == 1){
        //DECODE SECOND CONTROL BYTES
        for(i=((numberOfNodesToInt*16)+28); i<((numberOfNodesToInt*16)+36); i++){
            if(receivedData[i] != '0'){
                Printf("Second control bytes check failed\r");
                return 0;
            }
        }
        Printf("Same for all = yes\r");
    }else{
        //DECODE SECOND CONTROL BYTES
        for(i=((numberOfNodesToInt*24)+20); i<((numberOfNodesToInt*24)+28); i++){
            if(receivedData[i] != '0'){
                Printf("Second control bytes check failed\r");
                return 0;
            }
        }
        Printf("Same for all = no\r");
    }

    Printf("Second control bytes are correct\r");

    //DECODE INVOLVED NODES
    for(i=3; i<((numberOfNodesToInt*16)+3); i++){
        involvedNodes[i-3] = receivedData[i];
    }
    Printf("Involved nodes = ");
    if( ESP8266involvedNodes(involvedNodes) == 0){
            return 0;
    }

    //DECODE INVOLVED REGISTERS
    if(sameForAllToInt == 1){
       for(i=((numberOfNodesToInt*16)+20); i<((numberOfNodesToInt*16)+28); i++){
            involvedReg[i-((numberOfNodesToInt*16)+20)] = receivedData[i];
        } 
    }else{
        for(i=((numberOfNodesToInt*16)+20); i<((numberOfNodesToInt*24)+20); i++){
            involvedReg[i-((numberOfNodesToInt*16)+20)] = receivedData[i];
        }
    }
    Printf("Involved registers = ");
    ESP8266involvedReg(involvedReg);
    Printf("Value fields was discarded cause the type of the petition\r");
    
    return 1;
}

/*******************************************************************************
* Function:         UINT8 decodeReadLikeFunction(char* receivedData, int charcounter)
* Input:            Received Data and the length of that char*
* Output:           An int. 1 if the packet is correct formatted, 0 if doesn't
* Overview:         This function gets the entire packet and checks if all the
*                   fields are correct formatted. Only called when the packet
*                   has a READ SOME TYPE structure, you can see the structure type of
*                   the packet in caseXXXXXXXX function. 
*******************************************************************************/
UINT8 decodeReadSomeLikeFunction(char* receivedData, int charcounter){
    UINT16 i; 
    
    if(numberOfNodesToInt == 0 || (numberOfNodesToInt != 1 && numberOfNodesToInt != 0xF) ){
        Printf("Incorrect node amount \r ");
        return 0;
    }
    Printf("Number of nodes = ");
    if(numberOfNodesToInt == 0xF){
        Printf("BROADCAST");
    } else {
        PrintDec(numberOfNodesToInt);
    }
    Printf("\r");

    //Make this first and if they are correct, then get the nodes ID, registers and values
    //DECODE FIRST CONTROL BYTES
    for(i=19; i<35; i++){
        if(receivedData[i] != '0'){
            Printf("First control bytes check failed\r");
            return 0;
        }
    }
    Printf("First control bytes are correct\r");

    //DECODE SAME REGISTER FOR ALL
    sameForAll = receivedData[35];
    sameForAllToInt = ESP8266sameForAll(sameForAll);
    if(sameForAllToInt == 10 || sameForAllToInt == 0){
        Printf("Unknow Same for all value");
        return 0;
    }

    //DECODE SECOND CONTROL BYTES
    for(i=(36+(sameForAllToInt*8)); i<(44+(sameForAllToInt*8)); i++){
        if(receivedData[i] != '0'){
            Printf("Second control bytes check failed\r");
            return 0;
        }
    }
    Printf("Second control bytes are correct\r");

    //DECODE INVOLVED NODES
    for(i=3; i<19; i++){
        involvedNodes[i-3] = receivedData[i];
    }
    
    if(numberOfNodesToInt != 0xF){
        Printf("Involved nodes = ");
        if( ESP8266involvedNodes(involvedNodes) == 0){
                return 0;
        }
    }
    Printf("Same for all = ");
    PrintDec(sameForAllToInt);
    Printf("\r");

    //DECODE INVOLVED REGISTERS
    for(i=36; i<(36+(sameForAllToInt*8)); i++){
        involvedReg[i-36] = receivedData[i];
    }
    Printf("Involved registers = ");
    ESP8266involvedReg(involvedReg);
    Printf("Value fields was discarded cause the type of the petition\r");
    
    return 1;
}

/*******************************************************************************
* Function:         UINT8 decodeReadLikeFunction(char* receivedData, int charcounter)
* Input:            Received Data and the length of that char*
* Output:           An int. 1 if the packet is correct formatted, 0 if doesn't
* Overview:         This function gets the entire packet and checks if all the
*                   fields are correct formatted. Only called when the packet
*                   has a WRITE TYPE structure, you can see the structure type of
*                   the packet in caseXXXXXXXX function. 
*******************************************************************************/
UINT8 decodeWriteLikeFunction(char* receivedData, int charcounter){
    UINT16 i;
    
    if(numberOfNodesToInt == 0 || numberOfNodesToInt == 0xF){
        Printf("Incorrect node amount \r ");
        return 0;
    }
    Printf("Number of nodes = ");
    PrintDec(numberOfNodesToInt);
    Printf("\r");

    //Make this first and if they are correct, then get the nodes ID, registers and values
    //DECODE FIRST CONTROL BYTES
    for(i=((numberOfNodesToInt*16)+3); i<((numberOfNodesToInt*16)+19); i++){
        if(receivedData[i] != '0'){
            Printf("First control bytes check failed\r");
            return 0;
        }
    }
    Printf("First control bytes are correct\r");

    //DECODE SAME REGISTER FOR ALL
    sameForAll = receivedData[((numberOfNodesToInt*16)+19)];
    sameForAllToInt = ESP8266sameForAll(sameForAll);

    if(sameForAllToInt == 0 || (sameForAllToInt != 1 && sameForAllToInt != 10) ){
        Printf("Unknow Same for all value");
        return 0;
    }

    //DECODE SECOND CONTROL BYTES
    if(sameForAllToInt == 1){
        for(i=((numberOfNodesToInt*16)+28); i<((numberOfNodesToInt*16)+36); i++){
            if(receivedData[i] != '0'){
                Printf("Second control bytes check failed\r");
                return 0;
            }
        }
        Printf("Same for all = yes\r");
    }else{
        for(i=((numberOfNodesToInt*24)+20); i<((numberOfNodesToInt*24)+28); i++){
            if(receivedData[i] != '0'){
                Printf("Second control bytes check failed\r");
                return 0;
            }
        }
        Printf("Same for all = no\r");
    }

    Printf("Second control bytes are correct\r");

    //DECODE INVOLVED NODES
    for(i=3; i<((numberOfNodesToInt*16)+3); i++){
        involvedNodes[i-3] = receivedData[i];
    }
    Printf("Involved nodes = ");
    if( ESP8266involvedNodes(involvedNodes) == 0){
            return 0;
    }

    //DECODE INVOLVED REGISTERS
    if(sameForAllToInt == 1){
        for(i=((numberOfNodesToInt*16)+20); i<((numberOfNodesToInt*16)+28); i++){
            involvedReg[i-((numberOfNodesToInt*16)+20)] = receivedData[i];
        } 
    }else{
        for(i=((numberOfNodesToInt*16)+20); i<((numberOfNodesToInt*24)+20); i++){
            involvedReg[i-((numberOfNodesToInt*16)+20)] = receivedData[i];
        }
    }
    Printf("Involved registers = ");
    ESP8266involvedReg(involvedReg);

    //DECODE VALUES
    if(sameForAllToInt == 1){
        if(charcounter !=(((numberOfNodesToInt*16)+36) + (numberOfNodesToInt*8)+1)) {
            Printf("Incorrect amount of values\r");
            return 0;
        }
        for(i=((numberOfNodesToInt*16)+36); i<charcounter; i++){
            value[i-((numberOfNodesToInt*16)+36)] = receivedData[i];
        }
        Printf("Values = ");
        ESP8266value(value); 
    }else{
        if(charcounter !=((((numberOfNodesToInt*24)+28) + 8*numberOfNodesToInt + 1))){
            Printf("Incorrect amount of values\r");
            return 0;
        }
        for(i=((numberOfNodesToInt*24)+28); i<charcounter; i++){
            value[i-((numberOfNodesToInt*24)+28)] = receivedData[i];
        }
        Printf("Values = ");
        ESP8266value(value); 
    }
    
    return 1;
}

/*******************************************************************************
* Function:         UINT8 decodeReadLikeFunction(char* receivedData, int charcounter)
* Input:            Received Data and the length of that char*
* Output:           An int. 1 if the packet is correct formatted, 0 if doesn't
* Overview:         This function gets the entire packet and checks if all the
*                   fields are correct formatted. Only called when the packet
*                   has a WRITE SOME TYPE structure, you can see the structure type of
*                   the packet in caseXXXXXXXX function. 
*******************************************************************************/
UINT8 decodeWriteSomeLikeFunction(char* receivedData, int charcounter){
    UINT16 i;
    
    if(numberOfNodesToInt == 0 || (numberOfNodesToInt != 1 && numberOfNodesToInt != 0xF) ){
        Printf("Incorrect node amount \r ");
        return 0;
    }
    Printf("Number of nodes = ");
    if(numberOfNodesToInt == 0xF){
        Printf("BROADCAST");
    } else {
        PrintDec(numberOfNodesToInt);
    }
    Printf("\r");

    //Make this first and if they are correct, then get the nodes ID, registers and values
    //DECODE FIRST CONTROL BYTES
    for(i=19; i<35; i++){
        if(receivedData[i] != '0'){
            Printf("First control bytes check failed\r");
            return 0;
        }
    }
    Printf("First control bytes are correct\r");

    //DECODE SAME REGISTER FOR ALL
    sameForAll = receivedData[35];
    sameForAllToInt = ESP8266sameForAll(sameForAll);
    if(sameForAllToInt == 10 || sameForAllToInt == 0){
        Printf("Unknow Same for all value");
        return 0;
    }

    //DECODE SECOND CONTROL BYTES
    for(i=(36+(sameForAllToInt*8)); i<(44+(sameForAllToInt*8)); i++){
        if(receivedData[i] != '0'){
            Printf("Second control bytes check failed\r");
            return 0;
        }
    }
    Printf("Second control bytes are correct\r");

    //DECODE INVOLVED NODES
    for(i=3; i<19; i++){
        involvedNodes[i-3] = receivedData[i];
    }
    Printf("Involved nodes = ");
    if( ESP8266involvedNodes(involvedNodes) == 0){
            return 0;
    }

    Printf("Same for all = ");
    PrintDec(sameForAllToInt);
    Printf("\r");

    //DECODE INVOLVED REGISTERS
    for(i=36; i<(36+(sameForAllToInt*8)); i++){//((numberOfNodesToInt*(24+sameForAllToInt))+20); i++){
        involvedReg[i-36] = receivedData[i];
    }
    Printf("Involved registers = ");
    ESP8266involvedReg(involvedReg);

    if(charcounter != (44+(sameForAllToInt*8) + (sameForAllToInt*8) +1) ){
        Printf("Incorrect amount of values\r");
        return 0;
    }

    //DECODE VALUES
    for(i=(44+(sameForAllToInt*8)); i<charcounter; i++){
        value[i-(44+(sameForAllToInt*8))] = receivedData[i];
    }
    Printf("Values = ");
    ESP8266value(value); 
    Printf("\r");
    
    return 1;
}

/*******************************************************************************
* Function:         UINT8 ESP8266SendCommands(BYTE lessNoisyChannel, radioInterface rx)
* Input:            The less noisy channel, found in a previous scan and the
*                   radio interface where it will be send. 
* Output:           An int. 1 if the packet is correct send, 0 if doesn't
* Overview:         This function sends the packet to the correspondant nodes
*******************************************************************************/
UINT8 ESP8266SendCommands(BYTE lessNoisyChannel, radioInterface rx){
    UINT16 i;
    UINT8 j;
    UINT8 l;
    UINT8 k = 0;
    BYTE sum;
    char temp[2];
    
    DiscardTXData(rx);
    
    packetToSend[0]= instructionCode[0];
    packetToSend[1]= instructionCode[1];
    
    if( (instructionCodeToInt == READ_SOME) || (instructionCodeToInt == WRITE_SOME) || (instructionCodeToInt == MONITOR_SOME) || (instructionCodeToInt == NO_MONITOR_SOME) ||
            (instructionCodeToInt == NOTIFY_SOME) || (instructionCodeToInt == NO_NOTIFY_CHANGE_SOME) || (instructionCodeToInt == GET_TRACES)){
        k = 0;
        //MAKE THE PACKET TO BE SEND AS INSTRUCTION CODE + INVOLVED REGS + CONTROL BYTES + INVOLVED VALUES
        for(i=0; i<sizeof(involvedReg); i++){
            if(involvedReg[i] != NULL){
                packetToSend[i+2] = involvedReg[i];
                k=i;
            }
        }
        for(i=0; i<8; i++){
            packetToSend[k+3] = '0';
            k++;
        }
        
        for(i=0; i<sizeof(value); i++){
            if(value[i] != NULL){
                packetToSend[k+3] = value[i];
                k++;
            }
        }
        
        packetToSendTemp[0] = '8';
        packetToSendTemp[1] = '0';

        for(i=0; i<PACKET_SIZE; i++){
            if(packetToSend[i] != NULL){
                packetToSendTemp[i+2] = packetToSend[i];
            }
        }

        for(i=0; i<1024; i++){
            if(packetToSendTemp[i] != NULL){
                packetToSend[i] = packetToSendTemp[i];
            }
        }
        
        Printf("Packet to send formatted as: ");
        Printf(packetToSend);
        Printf("\r");
        
        //WRITING THE PACKET IN THE TX BUFFER
        l=0;
        while(l < sizeof(packetToSend)){
            if(packetToSend[l] != NULL){
                j = PutTXData(rx, packetToSend[l]);
            } else {
                break;
            }
            if (j){
                Printf("\r\nFailure writing buffer. Packet is too large. Error code: ");
                PrintChar(j);
                Printf("\r");
                return 0;
            }else{
                l++;
            }
        }
       
        //SEND THE PACKET IF BROADCAST
        if(numberOfNodesToInt == 0xF){
            while(TRUE){
                i = SendPckt(rx, BROADCAST_ADDRMODE, NULL);
                if (i != NO_ERROR){
                    Printf("Failure sending broadcast packet.\r");
                    ESP8266ClearDataToSend();
                    return 0;
                }
                else{
                    Printf("Broadcast packet sent correctly.\r");
                    ESP8266ClearDataToSend();
                    return 1;
                }
            }
        }
    
        //SEND THE PACKET IF UNICAST
        else if(numberOfNodesToInt == 1){
            for(j=0; j<16; j++){
                temp[0] = involvedNodesSplit[0][j];
                temp[1] = involvedNodesSplit[0][j];
                sum = (int)strtol(temp, NULL, 16);
                //sum = atoi(temp);
                if(j == 0){
                    addressToSend [j] = sum;
                } else {
                  addressToSend [j/2] = sum;
                }
                j++;
            }
            while(TRUE){
                //Enviar paquete con los datos que haya en el buffer de la interfaz
                i = SendPckt(rx, LONG_MIWI_ADDRMODE, addressToSend);
                if (i==0){
                    Printf("Unicast packet sent correctly.\r");
                    return 1;
                }
                else{
                    Printf("Failure sending unicast packet.\r");
                    return 0;
                }
            }
        }
        Printf("Something goes wrong. Number of nodes: ");
        PrintDec(numberOfNodesToInt);
        Printf("\r");
        return 0;
    }
    
    else if( (instructionCodeToInt == READ) || (instructionCodeToInt == WRITE) || (instructionCodeToInt == MONITOR) || (instructionCodeToInt == NO_MONITOR) ||
            (instructionCodeToInt == NOTIFY_CHANGE) || (instructionCodeToInt == NO_NOTIFY_CHANGE) || (instructionCodeToInt == RESET_NODE)){
        
        //MAKE THE PACKET TO BE SEND AS INSTRUCTION CODE + INVOLVED REGS + CONTROL BYTES + INVOLVED VALUES (EXCEPT RESET)
        for(i=0; i<numberOfNodesToInt; i++){
            if((instructionCodeToInt != RESET_NODE)) {
                k = 0;
                for(j=0; j<8; j++){
                    if(sameForAllToInt == 10){
                        if(involvedRegSplit[i][j] != NULL){
                            packetToSend[j+2] = involvedRegSplit[i][j];
                            k=j;
                        }
                    } else if( sameForAllToInt == 1){
                        if(involvedRegSplit[0][j] != NULL){
                            packetToSend[j+2] = involvedRegSplit[0][j];
                            k=j;
                        }
                    }
                }
                for(j=0; j<8; j++){
                    packetToSend[k+3]= '0';
                    k++;
                }
                for(j=0; j<8; j++){
                    if(valueSplit[i][j] != NULL){
                        packetToSend[k+3]= valueSplit[i][j];
                        k++;
                    }
                }
            }
            
            packetToSendTemp[0] = '8';
            packetToSendTemp[1] = '0';
            
            for(i=0; i<PACKET_SIZE; i++){
                if(packetToSend[i] != NULL){
                    packetToSendTemp[i+2] = packetToSend[i];
                }
            }
            
            for(i=0; i<1024; i++){
                if(packetToSendTemp[i] != NULL){
                    packetToSend[i] = packetToSendTemp[i];
                }
            }
            
            Printf("Packet to send formatted as: ");
            Printf(packetToSend);
            Printf("\r");

            //WRITING THE PACKET IN THE TX BUFFER
            l=0;
            while(l < sizeof(packetToSend)){
                if(packetToSend[l] != NULL){
                    j = PutTXData(rx, packetToSend[l]);
                } else {
                    break;
                }
                if (j){
                    Printf("\rFailure writing buffer. Packet is too large. Error code: ");
                    PrintChar(j);
                    Printf("\r");
                    return 0;
                }else{
                    l++;
                }
            }

            for(j=0; j<16; j++){
                temp[0] = involvedNodesSplit[0][j];
                temp[1] = involvedNodesSplit[0][j];
                sum = (int)strtol(temp, NULL, 16);
                //sum = atoi(temp);
                if(j == 0){
                    addressToSend [j] = sum;
                } else {
                  addressToSend [j/2] = sum;
                }
                j++;
            }
            /*
            Printf("Address to send:");
            for(j=0; j<8; j++){
                Print32Dec(addressToSend[j]);
            }
            Printf("\r");
            */
            //Enviar paquete con los datos que haya en el buffer de la interfaz
            j = SendPckt(rx, LONG_MIWI_ADDRMODE, addressToSend);
            if (j==0){
                Printf("Multicast packet sent correctly.\r");
            }
            else{
                Printf("Failure sending multicast packet.\r");
                return 0;
            }
        }
        return 1;
    }
    
    else if(instructionCodeToInt == BOOTLOADER){
        //PLACE HERE THE CODE TO BOOTLOADER
    }
    
    return 0;
}

/*******************************************************************************
* Function:         UINT8 asciiToHex(char ch)
* Input:            a char to convert in hexadecimal format
* Output:           The char converted in hex
* Overview:         Converts an ASCII char in the correspondant hex value. For
*                   example, '1' in 1, '2' in 2, etc...
*******************************************************************************/
UINT8 asciiToHex(char ch){
    if(ch == '1'){
        return 0x1;
    } else if(ch == '2'){
        return 0x2;
    } else if(ch == '3'){
        return 0x3;
    } else if(ch == '4'){
        return 0x4;
    } else if(ch == '5'){
        return 0x5;
    } else if(ch == '6'){
        return 0x6;
    } else if(ch == '7'){
        return 0x7;
    } else if(ch == '8'){
        return 0x8;
    } else if(ch == '9'){
        return 0x9;
    } else if(ch == '0'){
        return 0x0;
    } else if((ch == 'A') || (ch == 'a')){
        return 0xA;
    } else if((ch == 'B') || (ch == 'b')){
        return 0xB;
    } else if((ch == 'C') || (ch == 'c')){
        return 0xC;
    } else if((ch == 'D') || (ch == 'd')){
        return 0xD;
    } else if((ch == 'E') || (ch == 'e')){
        return 0xE;
    } else if((ch == 'F') || (ch == 'f')){
        return 0xF;
    }
    return 0xFF;
}

/*******************************************************************************
* Function:         void ESP8266ClearData(void)
* Input:            none
* Output:           none
* Overview:         Clean all the array variables
*******************************************************************************/
void ESP8266ClearData(void){
    UINT8 i;
    UINT8 j;
    
    for(i = 0; i<sizeof(involvedNodes); i++){
        involvedNodes[i] = NULL;
    }
    for(i = 0; i<sizeof(involvedReg); i++){
        involvedReg[i] = NULL;
        value[i] = NULL;
    }
    for(i = 0; i<7; i++){
        for(j = 0; j<16; j++){
            involvedNodesSplit[i][j] = NULL;
        }
    }
    for(i = 0; i<7; i++){
        for(j = 0; j<8; j++){
            involvedRegSplit[i][j];
            valueSplit[i][j];
        }
    }
}

/*******************************************************************************
* Function:         void ESP8266ClearData(void)
* Input:            none
* Output:           none
* Overview:         Clean all the array variables used to send a packet
*******************************************************************************/
void ESP8266ClearDataToSend(void){
    UINT16 i;
    for(i=0; i<sizeof(packetToSend); i++){
        packetToSend[i] =  NULL;
        packetToSendTemp[i] =  NULL;
    }
    packetToSendTemp[PACKET_SIZE] =  NULL;
    packetToSendTemp[PACKET_SIZE+1] =  NULL;
    
    for(i=0; i<sizeof(addressToSend); i++){
        addressToSend[i] =  NULL;
    }
}
#if defined TEST_NODE

void testButtonsAndLeds(void){
    //readValue = ReadBUTTONS();
    if(BUTTON1_PUSHED == TRUE){
        HEADER_01 = 1;
            LED1 = 1;
            LED2 = 1;
            LED3 = 1;
            BUTTON1_PUSHED = FALSE;
    }
    if(BUTTON2_PUSHED == TRUE){
        HEADER_01 = 0;
        LED1 = 0;
        LED2 = 0;
        LED3 = 0;
        BUTTON2_PUSHED = FALSE;
    }
}
#endif

#else
    #error Unknown processor.  See Compiler.h
#endif

#endif  //ENABLE_ESP8266
