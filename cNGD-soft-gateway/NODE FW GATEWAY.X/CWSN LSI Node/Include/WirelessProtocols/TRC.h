/*******************************************************************************
* FileName:	TRC.h
* Dependences: See INCLUDES section
* Author:  David Torres Retamosa - B105 - UPM
********************************************************************************
* File Description:
* This header file allows the communication between a regular node and a gateway
* node.
*******************************************************************************/

#ifndef  _TRC_H_
#define  _TRC_H_

/********************************* HEADERS ************************************/
#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "ConfigApp.h"
#include "HardwareProfile.h"
#include "../NodeHAL.h"

/******************************* DEFINITIONS **********************************/

#if defined(__dsPIC30F__) || defined(__dsPIC33F__) || defined(__PIC24F__) || \
    defined(__PIC24FK__)  || defined(__PIC24H__)   || defined(__PIC32MX__)
   
    typedef struct Registers {
        UINT8  notify_flag;
        UINT8  monitor_flag;
        unsigned char name[8];
        UINT32 value;
        UINT32 monitor_time_ms;
        UINT32 last_time_ms;
    } Register;
    
    UINT8 GatewayCheckHex(char* receivedData, radioInterface rix, BYTE address[8]);
    UINT8 GatewayDecodeData(char* receivedData, BYTE i);
    
    UINT8 RegisterInit(char* name, UINT32 value);
    
    UINT32 GetRegisterValue(char* name);
    UINT8 GetChangeFlag(char* name);
    UINT8 GetMonitorFlag(char* name);
    UINT32 GetMonitorTime(char* name);
    
    UINT8 SetRegisterValue(char* name, UINT32 value);
    UINT8 SetChangeFlag(char* name, UINT8 value);
    UINT8 SetMonitorFlag(char* name, UINT8 value);
    UINT8 SetRegisterName(char* old_name, char* new_name);
    UINT8 SetMonitorTime(char* name, UINT32 time_ms);
    
    void GatewayCopyTables(void);
    UINT8 GetRegistersAndValues(UINT8 regAndValue, UINT8 numberOfThem, char* receivedData);
    
    void PrintInvolvedRegs(void);
    void PrintInvolvedValues(void);
    void PrintPacketToSend(void);
    
    UINT8 GatewayExecuteChanges(radioInterface rix, BYTE address[8]);
    UINT8 sendPacket(radioInterface rix, BYTE address[8], char* packetToSend);
    UINT8 sendError(radioInterface rix, BYTE address[8], UINT8 numberOfChars, char char1, char char2, char char3);
    UINT8 formatPacket(UINT8 packetType, UINT8 numberOfChars, char char1, char char2, char char3);
    
    void GatewayCheckTimers(radioInterface rix, BYTE address[8]);
    void GatewayCheckChanges(radioInterface rix, BYTE address[8]);
    void GatewayClearData(void);
    UINT8 WriteTraces(char traces[8]);
    
    
#else
    #error Unknown processor.  See Compiler.h
#endif

        
#endif