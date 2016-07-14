/*******************************************************************************
* FileName:	Esp8266ConfigValidation.h
* Dependencies: See INCLUDES section
* Author:  David Torres Retamosa - B105 - UPM
********************************************************************************
* File Description:
* This file configures UART for ESP8266 and avoid collisions with other serial 
* communications ports.
*******************************************************************************/

#ifndef HW_VALIDATIONS_H
#define	HW_VALIDATIONS_H

#include "HardwareConfig.h"

////////////////////////////////////////////////////////////////////////////////
/* VALIDATIONS ****************************************************************/
////////////////////////////////////////////////////////////////////////////////

//UART validations.
#if defined USE_UART6 && defined DEBUG_UART6
    #error "UART6 is already used in RS232 communication"
#endif

#if defined USE_UART1  && (defined USE_UART2 || defined USE_UART3  || \
    defined USE_UART4  ||  defined USE_UART5 || defined USE_UART6) || \
    defined USE_UART2  && (defined USE_UART3 || defined USE_UART4  || \
    defined USE_UART5  || defined USE_UART6) || defined USE_UART3  && \
    (defined USE_UART4 || defined USE_UART5  || defined USE_UART6) || \
    defined USE_UART4  && (defined USE_UART5 || defined USE_UART6) || \
    (defined USE_UART5 && defined USE_UART6)
    #error "Select only one UART module for WIFI Module."
#endif

#if defined ENABLE_ESP8266 && !defined USE_UART1 && !defined USE_UART2 && \
    !defined USE_UART3   && !defined USE_UART4 && !defined USE_UART5 && \
    !defined USE_UART6
#error "Select one UART module for WIFI Module."
#endif

#if (defined MRF49XA_1_IN_SPI2 || defined MRF49XA_2_IN_SPI2 || \
    defined MRF89XA_IN_SPI2    || defined MRF24J40_IN_SPI2  || \
    defined MRF24WB0M_IN_SPI2)
    #if defined USE_UART3
    #error "UART3 and SPI2 modules are incompatible."
    #elif defined USE_UART6
    #define UART6_RXONLY
    #warning "UART6 and SPI2 share some pins. UART6 will be configured in "\
                 "RXonly mode, avoiding USEgin"
    #endif
#endif

#if (defined MRF49XA_1_IN_SPI3 || defined MRF49XA_2_IN_SPI3 || \
    defined MRF89XA_IN_SPI3    || defined MRF24J40_IN_SPI3  || \
    defined MRF24WB0M_IN_SPI3)
    #if defined USE_UART1
    #error "UART1 and SPI3 modules are incompatible."
    #elif defined USE_UART4
    #warning "UART4 and SPI3 share some pins. Check SPI3 is configured in "\
                 "normal mode and UART4 in RX only mode."
    #endif
#endif

#if defined MRF49XA_1_IN_SPI4 || defined MRF49XA_2_IN_SPI4 || \
    defined MRF24J40_IN_SPI4  || defined MRF89XA_IN_SPI4   || \
    defined MRF24WB0M_IN_SPI4
    #if defined USE_UART2
    #error "UART2 and SPI4 modules are incompatible."
    #elif defined USE_UART5
    #warning "UART5 and SPI4 share some pins. Check SPI4 is configured in "\
                 "normal mode and UART5 in RX only mode."
    #endif
#endif