/*******************************************************************************
* FileName:	TRC.c
* Dependencies: See INCLUDES section
* File Description:
* This file contains all the functions to allow the communication between a 
* regular node and a gateway one.
*******************************************************************************/

/********************************** HEADERS ***********************************/
#include "WirelessProtocols/Console.h"
#include "WirelessProtocols/TRC.h"
#include "WirelessProtocols/ConfigApp.h"
#include "Compiler.h"
#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "NodeHAL.h" 
#include "../Include/Common/timer.h"

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

#define TABLE_SIZE 50
#define PACKET_SIZE 2048

#define DEBUG

#ifndef DEBUG

#define Printf(x) Nop();

#endif

Register RegisterTable[TABLE_SIZE];
Register RegisterTableCopy[TABLE_SIZE];
static UINT8 lastPosition = 0;
static UINT16 tracesindex = 0;
static UINT8 insCode;
static UINT8 numberOfRegs;
static unsigned char instructionCode[] = {'0', '0'};
static unsigned char involvedRegSplit[7][9];
static unsigned char ValueSplit[7][9];
static unsigned char packetToSend[PACKET_SIZE];
static unsigned char regValueASCII[7][9];
static unsigned char traces[63][8];
static UINT32 timeelapsed = 0;

/*******************************************************************************
* Function:         UINT8 GatewayCheckHex(char* receivedData, radioInterface rix, BYTE address[8])
* Input:            The received data, the radio interface to send the error if
*                   has not hex chars, and the address to send that packet
* Output:           1 if all the characters are HEX, 0 if don't
* Overview:         Already explained
*******************************************************************************/
UINT8 GatewayCheckHex(char* receivedData, radioInterface rix, BYTE address[8]){
    UINT16 i;
    for(i=0; i<PACKET_SIZE; i++){
        if(receivedData[i] != NULL){
            if ('0' <= receivedData[i] && receivedData[i] <= '9') {

            } else if ('a' <= receivedData[i] && receivedData[i] <= 'f') {

            } else if ('A' <= receivedData[i] && receivedData[i] <= 'F') {

            } else {
                Printf("The packet has not hex characters. Discarded");
                Print32Dec(i);
                sendError(rix, address, 3, 'H', 'E', 'X');
                return 0;
            }
        } else {
            break;
        }
    }
    return 1;
    
}

/*******************************************************************************
* Function:         GatewayDecodeData(char* receivedData, BYTE i)
* Input:            The received data, and the length
* Output:           1 if the packet is correct formatted and 0 if doesn't 
* Overview:         Gets the instruction code and calls the
*                   getRegistersAndValues to get registers and values involved
*******************************************************************************/
UINT8 GatewayDecodeData(char* receivedData, BYTE i){
    instructionCode[0] = receivedData[0];
    instructionCode[1] = receivedData[1];
    
    //CHECK THE PACKET LENGTH
    
    if(instructionCode[0] == '1'){ //READ FUNCTIONS
        if(instructionCode[1] == '0'){
            insCode = READ;
            Printf("READ\r");
            return GetRegistersAndValues(1, 1, receivedData);
        } else if (instructionCode[1] == '1'){
            insCode = READ_SOME;
            Printf("READ SOME\r");
            return GetRegistersAndValues(1, 2, receivedData);
        }
        //PLACE HERE REST OF SPECIFIC READ FUNCTIONS
    } else if (instructionCode[0] == '2') {
        if(instructionCode[1] == '0'){
            insCode = WRITE;
            Printf("WRITE\r");
           return GetRegistersAndValues(2, 1, receivedData);
        } else if (instructionCode[1] == '1'){
            insCode = WRITE_SOME;
            Printf("WRITE SOME\r");
           return GetRegistersAndValues(2, 2, receivedData);
        }
        //PLACE HERE REST OF SPECIFIC WRITE FUNCTIONS
    } else if (instructionCode[0] == '3') {
        if(instructionCode[1] == '0'){
            insCode = MONITOR;
            Printf("MONITOR\r");
           return GetRegistersAndValues(2, 1, receivedData);
        } else if (instructionCode[1] == '1'){
            insCode = MONITOR_SOME;
            Printf("MONITOR SOME\r");
           return GetRegistersAndValues(2, 2, receivedData);
        } else if (instructionCode[1] == '2'){
            insCode = NO_MONITOR;
            Printf("NO MONITOR\r");
           return GetRegistersAndValues(1, 1, receivedData);
        } else if (instructionCode[1] == '3'){
            insCode = NO_MONITOR_SOME;
            Printf("NO MONITOR SOME\r");
           return GetRegistersAndValues(1, 2, receivedData);
        }
        //PLACE HERE REST OF SPECIFIC MONITOR FUNCTIONS
    } else if (instructionCode[0] == '4') {
        if(instructionCode[1] == '0'){
            insCode = NOTIFY_CHANGE;
            Printf("NOTIFY CHANGE\r");
           return GetRegistersAndValues(1, 1, receivedData);
        } else if (instructionCode[1] == '1'){
            insCode = NOTIFY_SOME;
            Printf("NOTIFY SOME CHANGES\r");
           return GetRegistersAndValues(1, 2, receivedData);
        } else if (instructionCode[1] == '2'){
            insCode = NO_NOTIFY_CHANGE;
            Printf("NO NOTIFY CHANGE\r");
           return GetRegistersAndValues(1, 1, receivedData);
        } else if (instructionCode[1] == '3'){
            insCode = NO_NOTIFY_CHANGE_SOME;
            Printf("NO NOTIFY SOME CHANGES\r");
           return GetRegistersAndValues(1, 2, receivedData);
        }
        //PLACE HERE REST OF SPECIFIC NOTIFICATION FUNCTIONS
    } else if (instructionCode[0] == '5') {
        if(instructionCode[1] == '0'){
            insCode = RESET_NODE;
            Printf("RESET NODE\r");
           return 1;
        }
        //PLACE HERE REST OF SPECIFIC RESET FUNCTIONS
    } else if (instructionCode[0] == '6') {
        if(instructionCode[1] == '0'){
            insCode = GET_TRACES;
            Printf("GET TRACES\r");
           return GetRegistersAndValues(1, 2, receivedData);;
        }
        //PLACE HERE REST OF SPECIFIC TRACES RECUPERATION FUNCTIONS
    } else if (instructionCode[0] == '7') {
        if(instructionCode[1] == '0'){
           insCode = BOOTLOADER;
           Printf("BOOTLOADER\r");
           return 1;
        }
        //PLACE HERE REST OF SPECIFIC BOOTLOADER FUNCTIONS
    }
    Printf("Incorrect instruction Code");
    return 0;
}

/*******************************************************************************
* Function:         RegisterInit(char name2[8], UINT32 value)
* Input:            a char array with the name and the value to be set
* Output:           1 if the creation was correct and 0 if doesn't 
* Overview:         Initialises a register. Fails if the name is too short, too
*                   long or already exist. Also fails if the register array is 
*                   full. All the flags, and monitor time will be set to zero
*******************************************************************************/
UINT8 RegisterInit(char name2[8], UINT32 value){ // char* name
    UINT8 i;
    
    if(lastPosition >= 49){
        Printf("Register table is full.\r");
        return 0;
    }
    for(i = 0; i<TABLE_SIZE; i++){
        if(strcmp(RegisterTable[i].name, name2) == 0){
            Printf("Already exist a register with name: ");
            Printf(name2);
            Printf("\r");
            return 0; 
        }
    }
    for(i=0; i<9; i++){
        if((name2[i] == NULL) && (i<8)){
          Printf("Name is too short: \r");
          Printf(name2);
          Printf("\r");
          return 0;  
        }
        if((name2[i] != NULL) && (i>7)){
          Printf("Name is too large: \r");
          Printf(name2);
          Printf("\r");
          return 0;  
        }
    }
    
    lastPosition++;
    RegisterTable[lastPosition].notify_flag = 0;
    RegisterTable[lastPosition].monitor_flag = 0;
    
    if(value == NULL){
        RegisterTable[lastPosition].value = 0x0;    
    } else {
        RegisterTable[lastPosition].value = value;
    }
    RegisterTable[lastPosition].monitor_time_ms = 0x0;
    RegisterTable[lastPosition].last_time_ms = 0x0;
    strcpy(RegisterTable[lastPosition].name, name2);
    
    
    Printf("Created register with name:");
    Printf(RegisterTable[lastPosition].name);
    Printf("\r");
    
    return 1;
}

/*******************************************************************************
* Function:         GetRegisterValue(char* name)
* Input:            The register name
* Output:           the value of that register or zero if doesnt exist
* Overview:         Get the value from a register. Fails if the register doesn't 
*                   exists.
*******************************************************************************/
UINT32 GetRegisterValue(char* name){
    int i;

    for(i = 0; i<TABLE_SIZE; i++){
        if(strcmp(RegisterTable[i].name, name) == 0){
            return RegisterTable[i].value; 
        }
    }
    Printf("No register with name: \r");
    Printf(name);
    Printf("\r");
    return 0x0;
}

/*******************************************************************************
* Function:         GetChangeFlag(char* name)
* Input:            The register name
* Output:           the change flag of that register or zero if doesnt exist
* Overview:         Get the flag from a register. Fails if the register doesn't 
*                   exists.
*******************************************************************************/
UINT8 GetChangeFlag(char* name){
    int i;
    for(i = 0; i<TABLE_SIZE; i++){
        if(strcmp(RegisterTable[i].name, name) == 0){
            return RegisterTable[i].notify_flag; 
        }
    }
    Printf("No register with that name\r");
    return 0xF;
}

/*******************************************************************************
* Function:         GetMonitorFlag(char* name)
* Input:            The register name
* Output:           the monitor flag of that register or zero if doesnt exist
* Overview:         Get the flag from a register. Fails if the register doesn't 
*                   exists.
*******************************************************************************/
UINT8 GetMonitorFlag(char* name){
    int i;
    for(i = 0; i<TABLE_SIZE; i++){
        if(strcmp(RegisterTable[i].name, name) == 0){
            return RegisterTable[i].monitor_flag; 
        }
    }
    Printf("No register with that name\r");
    return 0xF;
}

/*******************************************************************************
* Function:         GetMonitorTime(char* name)
* Input:            The register name
* Output:           the monitor time of that register or zero if doesnt exist
* Overview:         Get the monitor time from a register. Fails if the register doesn't 
*                   exists.
*******************************************************************************/
UINT32 GetMonitorTime(char* name){
    int i;
    for(i = 0; i<TABLE_SIZE; i++){
        if(strcmp(RegisterTable[i].name, name) == 0){
            return RegisterTable[i].monitor_time_ms; 
        }
    }
    Printf("No register with that name\r");
    return 0x0;
}

/*******************************************************************************
* Function:         SetRegisterValue(char* name, UINT32 value)
* Input:            The register name and the value to be set
* Output:           1 if the set was correct, 0 if doesn't
* Overview:         Set the value from a register. Fails if the register doesn't 
*                   exists.
*******************************************************************************/
UINT8 SetRegisterValue(char* name, UINT32 value){
    int i;
    for(i = 0; i<TABLE_SIZE; i++){
        if(strcmp(RegisterTable[i].name, name) == 0){
            RegisterTable[i].value =  value;
            Printf("Value set as ");
            Print32Dec(value);
            Printf(" (in decimal) in register: ");
            Printf(RegisterTable[i].name);
            Printf("\r");
            return 1; 
        }
    }
    Printf("No register with that name\r");
    return 0x0;
}

/*******************************************************************************
* Function:         SetChangeFlag(char* name, UINT8 value)
* Input:            The register name and the value to be set
* Output:           1 if the set was correct, 0 if doesn't
* Overview:         Set the change flag from a register. Fails if the register doesn't 
*                   exists.
*******************************************************************************/
UINT8 SetChangeFlag(char* name, UINT8 value){
    int i;
    for(i = 0; i<TABLE_SIZE; i++){
        if(strcmp(RegisterTable[i].name, name) == 0){
            if((RegisterTable[i].notify_flag == value) && (value == 0)){
                Printf("Change flag already have this value\r");
                return 0;
            } else {
                RegisterTable[i].notify_flag =  value;
                Printf("Change flag set to ");
                PrintDec(value);
                Printf(" (in decimal) in register: ");
                Printf(RegisterTable[i].name);
                Printf("\r");
                return 1;
            }
        }
    }
    Printf("No register with that name\r");
    return 0x0;
}

/*******************************************************************************
* Function:         SetMonitorFlag(char* name, UINT8 value)
* Input:            The register name and the value to be set
* Output:           1 if the set was correct, 0 if doesn't
* Overview:         Set the monitor flag from a register. Fails if the register doesn't 
*                   exists.
*******************************************************************************/
UINT8 SetMonitorFlag(char* name, UINT8 value){
    int i;
    for(i = 0; i<TABLE_SIZE; i++){
        if(strcmp(RegisterTable[i].name, name) == 0){
            if((RegisterTable[i].monitor_flag == value) && (value == 0)){
                Printf("Monitor flag already have this value\r");
                return 0; 
            } else {
                RegisterTable[i].monitor_flag = value;
                Printf("Monitor flag set as ");
                PrintDec(value);
                Printf(" (in decimal) in register: ");
                Printf(RegisterTable[i].name);
                Printf("\r");
                return 1;       
            }
        }
    }
    Printf("No register with that name\r");
    return 0x0;
}

/*******************************************************************************
* Function:         SetRegisterName(char* old_name, char new_name[8])
* Input:            The register name and the new name
* Output:           1 if the set was correct, 0 if doesn't
* Overview:         Set a new name to a register. Fails if the register doesn't 
*                   exists.
*******************************************************************************/
UINT8 SetRegisterName(char* old_name, char new_name[8]){
    UINT8 i;
    
    for(i=0; i<9; i++){
        if(new_name[i] == NULL && (i<8)){
          Printf("Name is too short\r");
            return 0;  
        }
        if(new_name[i] != NULL && (i>7)){
          Printf("Name is too large\r");
            return 0;  
        }
    }
    
    for(i = 0; i<TABLE_SIZE; i++){
        if(strcmp(RegisterTable[i].name, old_name) == 0){
            strcpy(RegisterTable[i].name, new_name);
            Printf("Name changed. New name: ");
            Printf(RegisterTable[i].name);
            Printf(". Old name: ");
            Printf(old_name);
            Printf("\r");
            return 1; 
        }
    }
    Printf("No register with that name\r");
    return 0x0;
}

/*******************************************************************************
* Function:         SetMonitorTime(char* name, UINT32 time_ms)
* Input:            The register name and the monitor time in ms
* Output:           1 if the set was correct, 0 if doesn't
* Overview:         Set the monitor time to a register, but no activates the 
*                   monitor flag.
*******************************************************************************/
UINT8 SetMonitorTime(char* name, UINT32 time_ms){
    int i;
    for(i = 0; i<TABLE_SIZE; i++){
        if(strcmp(RegisterTable[i].name, name) == 0){
            RegisterTable[i].monitor_time_ms = time_ms*61400; //Aprox, 61400 cycles is a ms.
            Printf("Monitor time set as ");
            Print32Dec(RegisterTable[i].monitor_time_ms);
            Printf(" (in decimal) in register: ");
            Printf(RegisterTable[i].name);
            Printf("\r");
            return 1; 
        }
    }
    Printf("No register with that name\r");
    return 0x0;
}

/*******************************************************************************
* Function:         GatewayCopyTables(void)
* Input:            none
* Output:           none
* Overview:         Copy the register table to another one, used to detect changes
*                   in the value fields
*******************************************************************************/
void GatewayCopyTables(void){
    int i;
    for(i=0; i<TABLE_SIZE; i++){
        RegisterTableCopy[i] = RegisterTable[i]; 
    }
}

/*******************************************************************************
* Function:         GetRegistersAndValues(UINT8 regAndValue, UINT8 numberOfThem, char* receivedData)
* Input:            regAndValue: 1 if only get the registers, 2 if also get the values
*                   numberOfThem: 1 if only get one register or register+value,
*                   2 if more than one. a char* with the received data
* Output:           1 if the process get well, 0 if doesn't
* Overview:         Get the registers and the values from the received packet
*******************************************************************************/
UINT8 GetRegistersAndValues(UINT8 regAndValue, UINT8 numberOfThem, char* receivedData){
    UINT16 i;
    UINT8 j;
    UINT16 k;
    
    if(regAndValue == 1){ //Only get regs (Read Type Petition)
        
        if(numberOfThem == 1){
            for(i=0; i<8; i++){
                involvedRegSplit[0][i] = receivedData[i+2];
            }
            for(i=10; i<18; i++){
                if(receivedData[i] != '0'){
                    return 0;
                }
            }
            return 1;
        } else {
            for(i=0; i<8; i++){
                for(j=0; j<8; j++){
                    involvedRegSplit[i][j] = receivedData[(i*8)+2+j];
                }
                involvedRegSplit[i][8] = NULL;
                
                if(involvedRegSplit[i][0] == '0' && involvedRegSplit[i][1] == '0' && involvedRegSplit[i][2] == '0' && involvedRegSplit[i][3] == '0' && involvedRegSplit[i][4] == '0'
                        && involvedRegSplit[i][5] == '0' && involvedRegSplit[i][6] == '0' && involvedRegSplit[i][7] == '0'){
                    for(j=0; j<8; j++){
                        involvedRegSplit[i][j] = NULL;
                    }
                    numberOfRegs = i;
                    i=8;
                    return 1;
                }   
            }
            return 0;
        }
        
    } else { //get regs and values (Write Type Petition)
        
        if(numberOfThem == 1){
            for(i=0; i<8; i++){
                involvedRegSplit[0][i] = receivedData[i+2];
            }
            for(i=10; i<18; i++){
                if(receivedData[i] != '0'){
                    return 0;
                }
            }
            for(i=18; i<26; i++){
                ValueSplit[0][i-18] = receivedData[i];
            }
            return 1;
        } else {
            for(i=0; i<8; i++){
                for(j=0; j<8; j++){
                    involvedRegSplit[i][j] = receivedData[(i*8)+2+j];
                }
                k=((i*8)+2+j);
                involvedRegSplit[i][8] = NULL;
                if(involvedRegSplit[i][0] == '0' && involvedRegSplit[i][1] == '0' && involvedRegSplit[i][2] == '0' && involvedRegSplit[i][3] == '0' && involvedRegSplit[i][4] == '0'
                   && involvedRegSplit[i][5] == '0' && involvedRegSplit[i][6] == '0' && involvedRegSplit[i][7] == '0'){
                    for(j=0; j<8; j++){
                        involvedRegSplit[i][j] = NULL;
                    }
                    numberOfRegs = i;
                    i=8;
                }   
            }
            for(i=0; i<numberOfRegs; i++){
                for(j=0; j<8; j++){
                    ValueSplit[i][j] = receivedData[k+(i*8)+j];//(numberOfRegs*8)+2+j];
                }  
                ValueSplit[i][8] = NULL;
            }
            return 1;
        }
       
    }
    return 0;
   
}

/*******************************************************************************
* Function:         void PrintInvolvedRegs(void)
* Input:            none
* Output:           none
* Overview:         Print the involved registers obtained in GetRegistersAndValues
*******************************************************************************/
void PrintInvolvedRegs(void){
    UINT8 i;
    UINT8 j;
    
    Printf("Involved registers: \r");
    
    for(i=0; i<7; i++){
        if(involvedRegSplit[i][7] != NULL){
            Printf(involvedRegSplit[i]);
            Printf("\r");
        }
    }
}

/*******************************************************************************
* Function:         void PrintInvolvedValues(void)
* Input:            none
* Output:           none
* Overview:         Print the involved values obtained in GetRegistersAndValues
*******************************************************************************/
void PrintInvolvedValues(void){
    UINT8 i;
    UINT8 j;
    
    Printf("Involved Values: \r");
    
    for(i=0; i<7; i++){
        if(ValueSplit[i][7] != NULL){
            Printf(ValueSplit[i]);
            Printf("\r");
        }
    }
}

/*******************************************************************************
* Function:         void PrintPacketToSend(void)
* Input:            none
* Output:           none
* Overview:         Print the packet that will be send to the gateway node
*******************************************************************************/
void PrintPacketToSend(void){
    UINT16 i;
    
    Printf("Packet to be send: \r");
    
    for(i=0; i<PACKET_SIZE; i++){
        if(packetToSend[i] != NULL){
            ConsolePut(packetToSend[i]);
        } else {
           // break;
        }
    }
    Printf("\r");
}

/*******************************************************************************
* Function:         UINT8 GatewayExecuteChanges(radioInterface rix, BYTE address[8])
* Input:            the radio interface to send the packet and the address to send
* Output:           1 if the execution was correct, 0 if doesnt
* Overview:         Executes all the changes requested in the received packet.
*                   Also, send the packet answering, calling sendPacket if
*                   everything goes well or sendError if doesn't.
*******************************************************************************/
UINT8 GatewayExecuteChanges(radioInterface rix, BYTE address[8]){
    UINT32 temp;
    UINT8 i;
    UINT8 j;
    UINT16 k;
    
    switch(insCode){
        case READ:
            if(formatPacket(1, 1, 'R', NULL, NULL)){
                return sendPacket(rix, address, packetToSend);
            }
            return sendError(rix, address, 1, 'R', NULL, NULL); 
            
        case READ_SOME:
            if(formatPacket(2, 2, 'R', 'S', NULL)){
                return sendPacket(rix, address, packetToSend);
            }
            return sendError(rix, address, 2, 'R', 'S', NULL); 
            
        case WRITE:
            sscanf(ValueSplit[0], "%x", &temp);
            if(SetRegisterValue(involvedRegSplit[0], temp)){
                temp = GetRegisterValue(involvedRegSplit[0]);
                sprintf(regValueASCII[0], "%08x", temp);
                if(formatPacket(1, 1, 'W', NULL, NULL)){
                     return sendPacket(rix, address, packetToSend);
                } 
            }
            return sendError(rix, address, 1, 'W', NULL, NULL);
            
        case WRITE_SOME:
           for(i=0;i<numberOfRegs; i++){
               sscanf(ValueSplit[i], "%x", &temp);
               if(!(SetRegisterValue(involvedRegSplit[i], temp))){
                   return sendError(rix, address, 2, 'W', 'S', NULL);
               } 
           }
           if(formatPacket(2, 2, 'W', 'S', NULL)){
                return sendPacket(rix, address, packetToSend);
           }
           return sendError(rix, address, 2, 'W', 'S', NULL);
           
        case MONITOR:
            sscanf(ValueSplit[i], "%d", &temp);
            if(SetMonitorTime(involvedRegSplit[0], temp)){
                if((SetMonitorFlag(involvedRegSplit[0], 1))){
                    if(formatPacket(1, 1, 'M', NULL, NULL)){
                        return sendPacket(rix, address, packetToSend);
                    }
                }
            }
            return sendError(rix, address, 1, 'M', NULL, NULL);
            
        case MONITOR_SOME:
            for(i=0;i<numberOfRegs; i++){
                sscanf(ValueSplit[i], "%d", &temp);
                if(SetMonitorTime(involvedRegSplit[i], temp)){
                    if(!(SetMonitorFlag(involvedRegSplit[i], 1))){
                        return sendError(rix, address, 2, 'M', 'S', NULL);
                    }
                } else {
                   return sendError(rix, address, 2, 'M', 'S', NULL); 
                }
            }
            if(formatPacket(2, 2, 'M', 'S', NULL)){
               return sendPacket(rix, address, packetToSend);
            }
            return sendError(rix, address, 2, 'M', 'S', NULL);
            
        case NO_MONITOR:
            if(SetMonitorTime(involvedRegSplit[0], 0x0)){
                if((SetMonitorFlag(involvedRegSplit[0], 0))){
                    if(formatPacket(1, 2, 'N', 'M', NULL)){
                        return sendPacket(rix, address, packetToSend);
                    }
                }
            }
            return sendError(rix, address, 2, 'N', 'M', NULL);
            
        case NO_MONITOR_SOME:
            for(i=0;i<numberOfRegs; i++){
                if(SetMonitorTime(involvedRegSplit[i], 0x0)){
                    if(!(SetMonitorFlag(involvedRegSplit[i], 0))){
                        return sendError(rix, address, 3, 'N', 'M', 'S');
                    }
                } else {
                   return sendError(rix, address, 3, 'N', 'M', 'S'); 
                }
            }
            if(formatPacket(2, 3, 'N', 'M', 'S')){
               return sendPacket(rix, address, packetToSend);
            }
            return sendError(rix, address, 3, 'N', 'M', 'S');
            
        case NOTIFY_CHANGE:
            if(SetChangeFlag(involvedRegSplit[0], 1)){
                if(formatPacket(1, 1, 'N', NULL, NULL)){
                    return sendPacket(rix, address, packetToSend);
                }
            }
            return sendError(rix, address, 1, 'N', NULL, NULL);
            
        case NOTIFY_SOME:
            for(i=0;i<numberOfRegs; i++){
                if(!(SetChangeFlag(involvedRegSplit[i], 1))){
                    return sendError(rix, address, 2, 'N', 'S', NULL); 
                }
            }
            if(formatPacket(2, 2, 'N', 'S', NULL)){
                return sendPacket(rix, address, packetToSend);
            }
            return sendError(rix, address, 2, 'N', 'S', NULL); 
            
        case NO_NOTIFY_CHANGE:
            if(SetChangeFlag(involvedRegSplit[0], 0)){
                if(formatPacket(1, 2, 'N', 'N', NULL)){
                    return sendPacket(rix, address, packetToSend);
                }
            }
            return sendError(rix, address, 2, 'N', 'N', NULL);
            
        case NO_NOTIFY_CHANGE_SOME:
            for(i=0;i<numberOfRegs; i++){
                if(!(SetChangeFlag(involvedRegSplit[i], 0))){
                    return sendError(rix, address, 3, 'N', 'N', 'S'); 
                }
            }
            if(formatPacket(2, 3, 'N', 'N', 'S')){
                return sendPacket(rix, address, packetToSend);
            }
            return sendError(rix, address, 3, 'N', 'N', 'S');
            
        case RESET_NODE:
            for(i=0; i<TABLE_SIZE; i++){
                if(RegisterTable[i].notify_flag == 1){
                    if(!(SetChangeFlag(RegisterTable[i].name, 0))){
                        return sendError(rix, address, 1, 'R', 'S', 'T');   
                    } 
                }
                if(RegisterTable[i].monitor_flag == 1){
                    if(!(SetMonitorFlag(RegisterTable[i].name, 0))){
                        return sendError(rix, address, 1, 'R', 'S', 'T');   
                    }
                    if(!(SetMonitorTime(RegisterTable[i].name, 0))){
                        return sendError(rix, address, 1, 'R', 'S', 'T');   
                    }
                }
            }
            packetToSend[0] = 'R';
            packetToSend[1] = 'S';
            packetToSend[2] = 'T';
            PrintPacketToSend();
            return sendPacket(rix, address, packetToSend);
        case GET_TRACES:
            for(i=0; i<63; i++){
                for(j=0; j<8; j++){
                    if(traces[i] != NULL){
                        packetToSend[k] = traces[i][j];
                        k++;
                    }
                }
            }
            packetToSend[k] = 'G';
            packetToSend[k+1] = 'T';
            return sendPacket(rix, address, packetToSend);
        case BOOTLOADER:
            //TO BE FILL BY WHO MAKES THE WOR MODULE
            return 1;
    }
}

/*******************************************************************************
* Function:         UINT8 sendPacket(radioInterface rix, BYTE address[8], char* packetToSend)
* Input:            Radio interface to send the packet, the address where send it
*                   and the packet to send
* Output:           1 if the send process was correct, 0 if doesnt
* Overview:         send the packet passed as parameter with the interface passed
*                   in the channel passed
*******************************************************************************/
UINT8 sendPacket(radioInterface rix, BYTE address[8], char* packetToSend){
    UINT8 i;
    UINT8 j;
    UINT16 l;
    UINT8 k = 0;
    
    //WRITING THE PACKET IN THE TX BUFFER
        l=0;
        while(l<PACKET_SIZE){
            if(packetToSend[l] != NULL){
                j = PutTXData(rix, packetToSend[l]);
            } else {
                //break;
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
        //SEND THE PACKET IF UNICAST
            while(TRUE){
                //Enviar paquete con los datos que haya en el buffer de la interfaz
                i = SendPckt(rix, LONG_MIWI_ADDRMODE, address);
                if (i==0){
                    Printf("Unicast packet sent correctly.\r");
                    return 1;
                }
                else{
                    Printf("Failure sending unicast packet.\r");
                    return 0;
                }
            }
        Printf("Something goes wrong.");
        return 0;
}

/*******************************************************************************
* Function:         UINT8 sendError(radioInterface rix, BYTE address[8], UINT8 numberOfChars, char char1, char char2, char char3)
* Input:            Radio interface to send the packet, the address where send it,
*                   the number of chars to add (maximum 3) and this 3 chars
* Output:           1 if the send process was correct, 0 if doesnt
* Overview:         Sends an error message, formed by ERR and 3 more chars passed
*                   as parameters.
*******************************************************************************/
UINT8 sendError(radioInterface rix, BYTE address[8], UINT8 numberOfChars, char char1, char char2, char char3){
    packetToSend[0] = 'E';
    packetToSend[1] = 'R';
    packetToSend[2] = 'R';
    switch(numberOfChars){
        case 1:
            packetToSend[3] = char1;
        case 2:
            packetToSend[3] = char1;
            packetToSend[4] = char2;
        case 3:
            packetToSend[3] = char1;
            packetToSend[4] = char2;
            packetToSend[5] = char3;
        default:
            break;
    }
    PrintPacketToSend();
    return sendPacket(rix, address, packetToSend);
}

/*******************************************************************************
* Function:         UINT8 formatPacket(UINT8 packetType, UINT8 numberOfChars, char char1, char char2, char char3)
* Input:            packetType: 1 if only have 1 registers and 1 value, 2 if have
*                   more than one. numberOfChars is the number of chars to be
*                   added in the packet that will be send, with them, we can 
*                   identify which instruction is about the packet. Also, the 
*                   chars.
* Output:           1 if the packet was correct formatted, 0 if doesn't
* Overview:         Formats the packet to be send.
*******************************************************************************/
UINT8 formatPacket(UINT8 packetType, UINT8 numberOfChars, char char1, char char2, char char3){ //1 normal 2, some
    UINT16 i, j, k;
    UINT32 temp;
    
    switch(packetType){
        case 1:
            if(temp = GetRegisterValue(involvedRegSplit[0])){
                sprintf(regValueASCII[0], "%08x", temp);
                k=0;
                for(i=0; i<24; i++){
                    if(i<8){
                        packetToSend[i] = involvedRegSplit[0][i];
                    }
                    if(i>7 && i<16){
                        packetToSend[i] = '0';
                    }
                    if(i>15 && i<24){
                       packetToSend[i] = regValueASCII[0][i-16]; 
                    }
                }
                k=24;
                break;
            }
            return 0;
            
        case 2:
            k=0;
            for(i=0; i<numberOfRegs; i++){
                for(j=0; j<8; j++){
                    packetToSend[(8*i)+j] = involvedRegSplit[i][j];
                }
            }
            k=(8*i)+j+1;
            for(i=k; i<k+8; i++){
                packetToSend[i] = '0';
            }
            k=(8*i)+j+9;
            for(i=0; i<numberOfRegs; i++){
                temp = GetRegisterValue(involvedRegSplit[i]);
                sprintf(regValueASCII[i], "%08x", temp);  
            }
            for(i=0; i<numberOfRegs; i++){
                for(j=0; j<8; j++){
                    packetToSend[k] = regValueASCII[i][j];
                    k++;
                }
            }
            break;
        default:
            Printf("Invalid packetTipe\r");
            return 0;
    }
    
    switch(numberOfChars){
        case 1:
            packetToSend[k++]= char1;
            break;
        case 2:
            packetToSend[k++]= char1;
            packetToSend[k++]= char2;
            break;
        case 3:
            packetToSend[k++]= char1;
            packetToSend[k++]= char2;
            packetToSend[k++]= char3;
            break;
        default:
            Printf("Invalid numberOfChars\r");
            return 0;
    }
    PrintPacketToSend();
    return 1;
}

/*******************************************************************************
* Function:         GatewayCheckTimers(radioInterface rix, BYTE address[8])
* Input:            The radio interface to send the packet and the address to
*                   send it
* Output:           none
* Overview:         Checks if the notification time has elapsed in registers
*                   marked with this option and if it is the case, send a packet
*                   with the actual value 
*******************************************************************************/
void GatewayCheckTimers(radioInterface rix, BYTE address[8]){
    UINT8 i;
    UINT8 j;
    UINT32 temp;
    
    timeelapsed = (ReadCoreTimer()-timeelapsed);
    for(i=0; i<TABLE_SIZE; i++){     
        if(RegisterTable[i].monitor_flag == 1){
            RegisterTable[i].last_time_ms += timeelapsed;
        }
    }
    
    for(j=0; j<TABLE_SIZE; j++){
        if((RegisterTable[j].monitor_flag == 1) && (RegisterTable[j].last_time_ms>=RegisterTable[j].monitor_time_ms)){
            temp = RegisterTable[j].value;
            sprintf(regValueASCII[0], "%08x", temp);
            strcpy(involvedRegSplit[0], RegisterTable[j].name);
            //involvedRegSplit[0] = RegisterTableCopy[j].name;
            for(i=0; i<24; i++){
                if(i<8){
                    packetToSend[i] = involvedRegSplit[0][i];
                }
                if(i<16 && i>7){
                    packetToSend[i] = '0';
                }
                if(i<24 && i>15){
                   packetToSend[i] = regValueASCII[0][i-16]; 
                }
            }
            packetToSend[24] = 'N';
            packetToSend[25] = 'T';
            PrintPacketToSend();
            sendPacket(rix, address, packetToSend);
            
            RegisterTable[j].last_time_ms = 0x0;
        }
    }
    timeelapsed = ReadCoreTimer();
}

/*******************************************************************************
* Function:         void GatewayCheckChanges(radioInterface rix, BYTE address[8])
* Input:            The radio interface to send the packet and the address to
*                   send it
* Output:           none
* Overview:         Checks if the value has changed in registers
*                   marked with this option and if it is the case, send a packet
*                   with the actual value 
*******************************************************************************/
void GatewayCheckChanges(radioInterface rix, BYTE address[8]){
    UINT8 i;
    UINT8 j;
    UINT32 temp;
    for(j=0; j< TABLE_SIZE; j++){
        if((RegisterTable[j].value != RegisterTableCopy[j].value) && (RegisterTableCopy[j].notify_flag == 1)){ 
            temp = RegisterTable[j].value;
            sprintf(regValueASCII[0], "%08x", temp);
            strcpy(involvedRegSplit[0], RegisterTableCopy[j].name);
            for(i=0; i<24; i++){
                if(i<8){
                    packetToSend[i] = involvedRegSplit[0][i];
                }
                if(i<16 && i>7){
                    packetToSend[i] = '0';
                }
                if(i<24 && i>15){
                   packetToSend[i] = regValueASCII[0][i-16]; 
                }
            }
            packetToSend[24] = 'C';
            packetToSend[25] = 'D';
            PrintPacketToSend();
            sendPacket(rix, address, packetToSend);
        }
    }
    GatewayCopyTables();
}

/*******************************************************************************
* Function:         void GatewayClearData(void)
* Input:            none
* Output:           none
* Overview:         Clears all the array variables used
*******************************************************************************/
void GatewayClearData(void){
    UINT16 i;
    UINT8 j;
    
    insCode = 0;
    numberOfRegs = 0;
    instructionCode[0] = 0;
    instructionCode[1] = 0;
    for(i=0; i<7; i++){
        for(j=0; j<8; j++){
           involvedRegSplit[i][j] = NULL;
           ValueSplit[i][j] = NULL; 
        }
    }
    for(i=0; i<sizeof(packetToSend); i++){
        packetToSend[i] = NULL;
    }
    
}

/*******************************************************************************
* Function:         UINT8 WriteTraces(char traces[8])
* Input:            The traces to write
* Output:           1 if the write was correct, 0 if doesn't
* Overview:         Writes a trace in the traces array. Returns 0 if the array
*                   is full, and 1 if the write process was correct.
*******************************************************************************/
UINT8 WriteTraces(char tracesValue[8]){
    UINT8 i;
    
    if(tracesindex >=62){
        Printf("Traces array is full\r");
        return 0;
    }
    for(i=0; i<8; i++){
        traces[tracesindex][i] = tracesValue[i];
    }
    tracesindex++;
    return 1;
}