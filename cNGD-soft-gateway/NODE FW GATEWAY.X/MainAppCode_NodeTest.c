/*******************************************************************************
 * File:   MainAppCode_NodeTest.c
 * Author: David Torres - B105 ESL - UPM
 ******************************************************************************/

#include "NodeHAL.h"
#include "WirelessProtocols/Console.h"  
#include "WirelessProtocols/ESP8266.h" //ESP8266 module

#define RECEIVED_SIZE 1024
//#define SOLDER_TEST
//#define BATTERY_TEST
#define REGULAR_NODE

#if defined SOLDER_TEST
    #if defined ENABLE_ESP8266
        #undef ENABLE_ESP8266
    #endif
    #if defined BATTERY_TEST
        #undef BATTERY_TEST
    #endif
    #if defined REGULAR_NODE
        #undef REGULAR_NODE
    #endif
#endif

#if defined BATTERY_TEST
    #if defined ENABLE_ESP8266
        #undef ENABLE_ESP8266
    #endif
    #if defined SOLDER_TEST
        #undef SOLDER_TEST
    #endif
    #if defined REGULAR_NODE
        #undef REGULAR_NODE
    #endif
#endif

#if defined REGULAR_NODE
    #if defined ENABLE_ESP8266
        #undef ENABLE_ESP8266
    #endif
    #if defined SOLDER_TEST
        #undef SOLDER_TEST
    #endif
    #if defined BATTERY_TEST
        #undef BATTERY_TEST
    #endif
#endif

#if defined MIWI_2400_RI
    radioInterface ri3 = MIWI_2400;
    BYTE NumChannels3 = MIWI2400NumChannels;
    BYTE powerStep3 = 8;
    BYTE ri_RI_MASK3 = MIWI_2400_RI_MASK;
#endif
#if defined MIWI_0868_RI
    radioInterface ri2 = MIWI_0868;
    BYTE NumChannels2 = MIWI0868NumChannels;
    BYTE powerStep2 = 32;
    BYTE ri_RI_MASK2 = MIWI_0868_RI_MASK;
#endif
#if defined MIWI_0434_RI
    radioInterface ri = MIWI_0434;
    BYTE NumChannels = MIWI0434NumChannels;
    BYTE powerStep = 32;
    BYTE ri_RI_MASK = MIWI_0434_RI_MASK;

    radioInterface ri1 = MIWI_0434;
    BYTE NumChannels1 = MIWI0434NumChannels;
    BYTE powerStep1 = 32;
    BYTE ri_RI_MASK1 = MIWI_0434_RI_MASK;
#endif

void InitAppVariables(){
    //Si las hubiera...
}

void PrintAddress(BYTE AddrMode, BYTE* Address){
    BYTE i;
    BYTE j = 0;
    Printf("\r\nAddrMode: ");
    switch(AddrMode){
        case LONG_MIWI_ADDRMODE:
            Printf("LONG_ADDR    Address: ");
            j = MY_ADDRESS_LENGTH;
            break;
        case SHORT_MIWI_ADDRMODE:
            Printf("SHORT_ADDR   Address: ");
            j = 2;
            break;
        default:
            Printf("ERROR");
            break;
    }
    for(i=0; i<j; i++){
        PrintChar(*(Address+j-1-i));
    }
}

char hexToAscii (BYTE hexchar){
    switch (hexchar){
        case 0x1:
            return '1';
        case 0x2:
            return '2';
        case 0x3:
            return '3';
        case 0x4:
            return '4';
        case 0x5:
            return '5';
        case 0x6:
            return '6';
        case 0x7:
            return '7';
        case 0x8:
            return '8';
        case 0x9:
            return '9';
        case 0x0:
            return '0';
        case 0xA:
            return 'A';
        case 0xB:
            return 'B';
        case 0xC:
            return 'C';
        case 0xD:
            return 'D';
        case 0xE:
            return 'E';
        case 0xF:
            return 'F';
    }
    
    return '0';
}

int mainApp(void) {
    
    UINT8 k;
    UINT16 charcounter = 0;
    UINT16 i,j;
    char receivedData[RECEIVED_SIZE];
    UINT8 decoderesult = 0;
    UINT8 sendresult = 0;
    UINT8 checkHex = 0;
    BYTE lessNoisyChannel, interface, hasData, data;
    BYTE rxAddress[8];
    radioInterface radio;
    UINT32 temp1;
    char rxAddressASCII[8][3];
    
    char loren[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\r";
    
    BYTE *beatles = "When I find myself in times of trouble, Mother Mary comes to me,"
                    " speaking words of wisdom, let it be. And in my hour of darkness"
                    " she is standing right in front of me, speaking words of wisdom,"
                    "let it be";
    
    InitAppVariables();
    InitNode();
    UINT32 time;

#if defined NODE_NOP
    while (1){      //Bucle infinito. Mantiene las pilas y no hace nada mas.
        #if defined NOP_JOINS_AND_LEAVES_LOOP
            SWDelay(6000);
            #if defined MIWI_0434_RI
                Printf("\r\nSearching for PANs and establishing connections "
                       "in 434 MHZ.");
                if(MiApp_SearchConne
                ction(9, 0xFFFFFFFF, MIWI_0434)){
                    MiApp_EstablishConnection(0, 0, MIWI_0434);
                }
                SWDelay(6000);
                DumpConnection(0xFF);
            #endif
        #elif defined NOP_DUMPS_CONN_TABLE
            SWDelay(5000);
            DumpConnection(0xFF);
        #endif
    }
#endif
    
#if defined ENABLE_ESP8266
    #undef NODE_1
    #define NODE_2

    ESP8266ClearData();
    ESP8266ClearDataToSend();
    Printf("\r\nScanning all interfaces. Searching the less noisy channel... Wait:\r");
    /*
    lessNoisyChannel = DoChannelScanning(ALL_MIWI, &interface);
    if(interface == MIWI_0434_RI_MASK){
        Printf("Transmission will in 434 MHz, in channel: ");
        radio = ri1;
    }else if(interface == MIWI_0868_RI_MASK){
        Printf("Transmission will in 868 MHz, in channel: ");
        radio = ri2;
    }else{
        Printf("Transmission will in 2,4 GHz, in channel: ");
        radio = ri3;
    }
     
    PrintDec(lessNoisyChannel);
    Printf("\r");
    */
    
    radio = ri3;
    lessNoisyChannel = GetOpChannel(ri3);
    /*
    Printf("Less Noisy Channel\r");
    PrintDec(lessNoisyChannel);
    Printf("\r");
    */
    //SetChannel(radio, lessNoisyChannel); //MODIFICADO!!!!! SetChannel(radio, lessNoisyChannel);
    
    while(HEADER_17 == 0);
    HEADER_01 = 1;
    while(HEADER_17 == 1){
       receivedData[charcounter] = ESP8266Get();
       charcounter++;
    }
    HEADER_01 = 0;
    Printf("IP of the gateway node is:");
    Printf(receivedData);
    Printf("\r");
    DelayMs(50);
    charcounter = 0;
    Printf("Waiting to receive a packet...\n");
    
    while(1){
       /************* CASE ESP8266 WANTS TO SEND DATA TO THE NODE **************/

       if(HEADER_17 == 1){

            /*RECEIVING DATA*/
            HEADER_01 = 1;
            while(HEADER_17 == 1){
                receivedData[charcounter] = ESP8266Get();
                charcounter++;
            }
            HEADER_01 = 0;

            /*PROCESSING DATA*/
            DelayMs(50);
            decoderesult = ESP8266Decode(receivedData, charcounter);
            DelayMs(50);
            charcounter = 0;

            /*ANSWERING TO ESP8266*/
            HEADER_01 = 1;
            while(HEADER_17 == 0);
            if(decoderesult == 1){
                SendViaESP8266("COMANDO RECIBIDO CORRECTAMENTE. EN BREVE SE ENVIARAN LOS DATOS");
            } else {
                ESP8266ClearData();
                SendViaESP8266("EL COMANDO NO SE RECONOCE Y POR TANTO HA SIDO DESCARTADO");
            }
            DelayMs(10);
            HEADER_01 = 0;
            DelayMs(100);

            /*SENDING THE COMMANDS TO THE INVOLVED NODES*/
            if(decoderesult == 1){
                sendresult = ESP8266SendCommands(lessNoisyChannel, radio);
                HEADER_01 = 1;
                while(HEADER_17 == 0);
                if(sendresult == 1){
                    SendViaESP8266("DATOS ENVIADOS A LOS NODOS CORRESPONDIENTES. A LA ESPERA DE RECIBIR RESPUESTA");
                } else {
                    ESP8266ClearData();
                    ESP8266ClearDataToSend();
                    SendViaESP8266("HA HABIDO ALGUN FALLO AL ENVIAR LOS DATOS");
                }
            }
            decoderesult = 0;
            sendresult = 0;
            DelayMs(10);
            HEADER_01 = 0;
            DelayMs(100);
            for(i=0; i<RECEIVED_SIZE; i++){
               receivedData[i] = NULL;
           }
       }

       /************* CASE NODE WANTS TO SEND DATA TO ESP8266 **************/

       i = WhichRIHasData();

       if (i & ri_RI_MASK1){
           
           DumpRXPckt(ri1);
           i = GetRXSourceAddr(ri1, rxAddress);
           j=0;
           
            for(i=0; i<8; i++){
                 sprintf(rxAddressASCII[i], "%02x", rxAddress[i]);
            }
            k=0;
            for(i=0; i<8; i++){
                 if(rxAddressASCII[k] != NULL){
                     receivedData[j] = rxAddressASCII[k][0];
                     j++;
                     receivedData[j] = rxAddressASCII[k][1];
                     j++;
                     k++;
                 }
            }
            while(GetPayloadToRead(ri1) > 0){
                GetRXData(ri1, &data);
                receivedData[j] = data;
                j++;
            }
            Printf("\rReceivedData (with RX address):\r");
            Printf(receivedData);
            Printf("\r");
                
            if((receivedData[16] == '8') && (receivedData[17] == '0')){ // THE RECEIVED PACKET COMES FROM THE GATEWAY
                HEADER_01 = 1;
                while(HEADER_17 == 0);
                SendViaESP8266(receivedData);
                DelayMs(10);
                HEADER_01 = 0;
                DelayMs(100);

                for(i=0; i<RECEIVED_SIZE; i++){
                    receivedData[i] = NULL;
                }
                ESP8266ClearData();
                ESP8266ClearDataToSend();
            } else {
           
            }

       }
       if (i & ri_RI_MASK2){
           
           DumpRXPckt(ri2);
           i = GetRXSourceAddr(ri2, rxAddress);
           j=0;
           
           
                for(i=0; i<8; i++){
                     sprintf(rxAddressASCII[i], "%02x", rxAddress[i]);
                }
                k=0;
                for(i=0; i<8; i++){
                     if(rxAddressASCII[k] != NULL){
                         receivedData[j] = rxAddressASCII[k][0];
                         j++;
                         receivedData[j] = rxAddressASCII[k][1];
                         j++;
                         k++;
                     }
                }
                while(GetPayloadToRead(ri2) > 0){
                    GetRXData(ri2, &data);
                    receivedData[j] = data;
                    j++;
                }
                Printf("\rReceivedData (with RX address):\r");
                Printf(receivedData);
                Printf("\r");
            if((receivedData[16] == '8') && (receivedData[17] == '0')){ // THE RECEIVED PACKET COMES FROM THE GATEWAY
                HEADER_01 = 1;
                while(HEADER_17 == 0);
                SendViaESP8266(receivedData);
                DelayMs(10);
                HEADER_01 = 0;
                DelayMs(100);

                for(i=0; i<RECEIVED_SIZE; i++){
                    receivedData[i] = NULL;
                }
                ESP8266ClearData();
                ESP8266ClearDataToSend();
            } else {

            }
       }
       if (i & ri_RI_MASK3){
           
           DumpRXPckt(ri3);
           i = GetRXSourceAddr(ri3, rxAddress);
           j=0;
           
           
                for(i=0; i<8; i++){
                     sprintf(rxAddressASCII[i], "%02x", rxAddress[i]);
                }
                k=0;
                for(i=0; i<8; i++){
                     if(rxAddressASCII[k] != NULL){
                         receivedData[j] = rxAddressASCII[k][0];
                         j++;
                         receivedData[j] = rxAddressASCII[k][1];
                         j++;
                         k++;
                     }
                }
                while(GetPayloadToRead(ri3) > 0){
                    GetRXData(ri3, &data);
                    receivedData[j] = data;
                    j++;
                }
                Printf("\rReceivedData (with RX address):\r");
                Printf(receivedData);
                Printf("\r");
                
        if((receivedData[16] == '8') && (receivedData[17] == '0')){ // THE RECEIVED PACKET COMES FROM THE GATEWAY
                HEADER_01 = 1;
                while(HEADER_17 == 0);
                SendViaESP8266(receivedData);
                DelayMs(10);
                HEADER_01 = 0;
                DelayMs(100);

                for(i=0; i<RECEIVED_SIZE; i++){
                    receivedData[i] = NULL;
                }
                
                ESP8266ClearData();
                ESP8266ClearDataToSend();
            } else {

            }
       }
       i = 0;
    }
#endif
    
#if defined REGULAR_NODE
    for(i=0; i<RECEIVED_SIZE; i++){
        receivedData[i] = NULL;
    }
    GatewayClearData();
    GatewayCopyTables();
    RegisterInit("123456785", 0x1000);
    RegisterInit("1234567", 0x1000);
    RegisterInit("12345678", 0x1000);
    RegisterInit("12345678", 0x1000);
    RegisterInit("11111111", 0x2000);
    RegisterInit("10000000", 0x3000);
    RegisterInit("12121212", 0x4000);
    RegisterInit("18888888", 0x5000);
    
    while(1){
        i = WhichRIHasData();

       if (i & ri_RI_MASK1){
           DumpRXPckt(ri1);
            i = GetRXSourceAddr(ri1, rxAddress);
            j=0;
            while(GetPayloadToRead(ri1) > 0){
                GetRXData(ri1, &data);
                receivedData[j] = data;
                j++;
            }
            Printf("\r");
            
            if((receivedData[0] == '8') && (receivedData[1] == '0')){ // THE RECEIVED PACKET COMES FROM THE GATEWAY
               
                for(i=0; i<(RECEIVED_SIZE-2); i++){
                    receivedData[i] = receivedData[i+2];
                }
                checkHex = GatewayCheckHex(receivedData, ri1, rxAddress);
                if(checkHex){
                     decoderesult = GatewayDecodeData(receivedData, i);
                     if(decoderesult){
                         Printf("Packet is correct. Making the requested operations...\r");
                         PrintInvolvedRegs();
                         Printf("\r");
                         PrintInvolvedValues();
                         Printf("\r");
                         sendresult = GatewayExecuteChanges(ri1, rxAddress);
                         if(sendresult){
                             Printf("Data sent\r");
                         } else {
                             Printf("Failure sending data\r");
                         }
                     } else {
                         Printf("Packet has errors. Discarded\r");
                     }
                     decoderesult = 0;
                     sendresult = 0;
                     checkHex = 0;
                 }
                 for(i=0; i<RECEIVED_SIZE; i++){
                     receivedData[i] = NULL;
                 }
                 GatewayClearData(ri1, rxAddress);
                 GatewayCheckTimers(ri1, rxAddress);
                 GatewayCheckChanges(ri1, rxAddress);
            } else {
                //PROCESS PACKET FOR OTHERS IMPLEMENTATIONS
                
            }

       } else if (i & ri_RI_MASK2){
           DumpRXPckt(ri2);
            i = GetRXSourceAddr(ri2, rxAddress);
            j=0;
            while(GetPayloadToRead(ri2) > 0){
                GetRXData(ri2, &data);
                receivedData[j] = data;
                j++;
            }
            Printf("\r");
            
            if((receivedData[0] == '8') && (receivedData[1] == '0')){ // THE RECEIVED PACKET COMES FROM THE GATEWAY
               
                for(i=0; i<(RECEIVED_SIZE-2); i++){
                    receivedData[i] = receivedData[i+2];
                }
                checkHex = GatewayCheckHex(receivedData, ri2, rxAddress);
                if(checkHex){
                     decoderesult = GatewayDecodeData(receivedData, i);
                     if(decoderesult){
                         Printf("Packet is correct. Making the requested operations...\r");
                         PrintInvolvedRegs();
                         Printf("\r");
                         PrintInvolvedValues();
                         Printf("\r");
                         sendresult = GatewayExecuteChanges(ri2, rxAddress);
                         if(sendresult){
                             Printf("Data sent\r");
                         } else {
                             Printf("Failure sending data\r");
                         }
                     } else {
                         Printf("Packet has errors. Discarded\r");
                     }
                     decoderesult = 0;
                     sendresult = 0;
                     checkHex = 0;
                 }
                 for(i=0; i<RECEIVED_SIZE; i++){
                     receivedData[i] = NULL;
                 }
                 GatewayClearData(ri2, rxAddress);
                 GatewayCheckTimers(ri2, rxAddress);
                 GatewayCheckChanges(ri2, rxAddress);
            } else {
                //PROCESS PACKET FOR OTHERS IMPLEMENTATIONS
                
            } 
       } else if (i & ri_RI_MASK3){
            DumpRXPckt(ri3);
            i = GetRXSourceAddr(ri3, rxAddress);
            j=0;
            while(GetPayloadToRead(ri3) > 0){
                GetRXData(ri3, &data);
                receivedData[j] = data;
                j++;
            }
            Printf("\r");
            
            if((receivedData[0] == '8') && (receivedData[1] == '0')){ // THE RECEIVED PACKET COMES FROM THE GATEWAY
               
                for(i=0; i<(RECEIVED_SIZE-2); i++){
                    receivedData[i] = receivedData[i+2];
                }
                checkHex = GatewayCheckHex(receivedData, ri3, rxAddress);
                if(checkHex){
                     decoderesult = GatewayDecodeData(receivedData, i);
                     if(decoderesult){
                         Printf("Packet is correct. Making the requested operations...\r");
                         PrintInvolvedRegs();
                         Printf("\r");
                         PrintInvolvedValues();
                         Printf("\r");
                         sendresult = GatewayExecuteChanges(ri3, rxAddress);
                         if(sendresult){
                             Printf("Data sent\r");
                         } else {
                             Printf("Failure sending data\r");
                         }
                     } else {
                         Printf("Packet has errors. Discarded\r");
                     }
                     decoderesult = 0;
                     sendresult = 0;
                     checkHex = 0;
                 }
                 for(i=0; i<RECEIVED_SIZE; i++){
                     receivedData[i] = NULL;
                 }
                 GatewayClearData(ri3, rxAddress);
                 GatewayCheckTimers(ri3, rxAddress);
                 GatewayCheckChanges(ri3, rxAddress);
            } else {
                //PROCESS PACKET FOR OTHERS IMPLEMENTATIONS
                
            }
        }
        i = 0;
    }
#endif
    
#if defined BATTERY_TEST
    /*
while(HEADER_17 == 0);
HEADER_01 = 1;
while(HEADER_17 == 1){
   receivedData[charcounter] = ESP8266Get();
   charcounter++;
}
HEADER_01 = 0;
Printf("IP of the gateway node is:");
Printf(receivedData);
Printf("\r");
DelayMs(50);
charcounter = 0;
Printf("Esperando a recibir un comando\n");
*/
while(1){
    DiscardTXData(ri1);
    DiscardTXData(ri2);
    DiscardTXData(ri3);
    Printf(loren);
    i=0;
    j=0;
    while(GetFreeTXBufSpace(ri1) > 0){     //Hasta llenar el buffer de transmision
        j = PutTXData(ri1, beatles[i]);
        if (j != NO_ERROR){
            Printf("\r\nFallo al escribir en el buffer: ");
            PrintChar(j);
        }
        else
            i++;
    }
    i=0;
    j=0;
    while(TRUE){
        //Enviar paquete con los datos que haya en el buffer de la interfaz
        i = SendPckt(ri1, BROADCAST_ADDRMODE, NULL);
        Printf("\r\nMiMAC_SendPacket devuelve: ");
        PrintChar(i);
        if (i != NO_ERROR){
            Printf(" => FALLO");
            break;
        }
        else{
            Printf(" => OK");
            break;  //TRANSMISION CORRECTA, SALE DEL BUCLE
        }
    }
    i=0;
    j=0;
    while(GetFreeTXBufSpace(ri2) > 0){     //Hasta llenar el buffer de transmision
        j = PutTXData(ri2, beatles[i]);
        if (j != NO_ERROR){
            Printf("\r\nFallo al escribir en el buffer: ");
            PrintChar(j);
        }
        else
            i++;
    }
    while(TRUE){
        //Enviar paquete con los datos que haya en el buffer de la interfaz
        i = SendPckt(ri2, BROADCAST_ADDRMODE, NULL);
        if (i != NO_ERROR){
            Printf(" => FALLO");
            break;
        }
        else{
            Printf(" => OK");
            break;  //TRANSMISION CORRECTA, SALE DEL BUCLE
        }
    }
    i=0;
    j=0;
    while(GetFreeTXBufSpace(ri3) > 0){     //Hasta llenar el buffer de transmision
        j = PutTXData(ri3, beatles[i]);
        if (j != NO_ERROR){
            Printf("\r\nFallo al escribir en el buffer: ");
            PrintChar(j);
        }
        else
            i++;
    }
    while(TRUE){
        //Enviar paquete con los datos que haya en el buffer de la interfaz
        i = SendPckt(ri3, BROADCAST_ADDRMODE, NULL);
        Printf("\r\nMiMAC_SendPacket devuelve: ");
        PrintChar(i);
        if (i != NO_ERROR){
            Printf(" => FALLO");
            break;
        }
        else{
            Printf(" => OK");
            break;  //TRANSMISION CORRECTA, SALE DEL BUCLE
        }
    }
/*
    HEADER_01 = 1;
    while(HEADER_17 == 0);
    SendViaESP8266(loren);
    DelayMs(10);
    HEADER_01 = 0;
    DelayMs(100);
  */
}
    
#if defined NODE_TEST
    
    
#endif
#endif

#if defined SOLDER_TEST
    while(1){
        testButtonsAndLeds();
    }
#endif
    return 0;
}



// Exception handler:
static enum {
    EXCEP_IRQ = 0,                  // interrupt
    EXCEP_AdEL = 4,                 // address error exception (load or ifetch)
    EXCEP_AdES,                     // address error exception (store)
    EXCEP_IBE,                      // bus error (ifetch)
    EXCEP_DBE,                      // bus error (load/store)
    EXCEP_Sys,                      // syscall
    EXCEP_Bp,                       // breakpoint
    EXCEP_RI,                       // reserved instruction
    EXCEP_CpU,                      // coprocessor unusable
    EXCEP_Overflow,                 // arithmetic overflow
    EXCEP_Trap,                     // trap (possible divide by zero)
    EXCEP_IS1 = 16,                 // implementation specfic 1
    EXCEP_CEU,                      // CorExtend Unuseable
    EXCEP_C2E                       // coprocessor 2
} _excep_code;

static unsigned int _epc_code;
static unsigned int _excep_addr;

// this function overrides the normal _weak_ generic handler
void _general_exception_handler(void){
    Printf("\r\n\nGeneral Exception Handler...\r\r");

    asm volatile("mfc0 %0,$13" : "=r" (_excep_code));
    asm volatile("mfc0 %0,$14" : "=r" (_excep_addr));

    _excep_code = (_excep_code & 0x0000007C) >> 2;
    while (1) {
        // Examine _excep_code to identify the type of exception
        // Examine _excep_addr to find the address that caused the exception
        Nop();
        Nop();
        Nop();
    }
}//End of exception handler
