/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18F27K42
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.36 and above
        MPLAB 	          :  MPLAB X 6.00	
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set LCD_CS aliases
#define LCD_CS_TRIS                 TRISAbits.TRISA0
#define LCD_CS_LAT                  LATAbits.LATA0
#define LCD_CS_PORT                 PORTAbits.RA0
#define LCD_CS_WPU                  WPUAbits.WPUA0
#define LCD_CS_OD                   ODCONAbits.ODCA0
#define LCD_CS_ANS                  ANSELAbits.ANSELA0
#define LCD_CS_SetHigh()            do { LATAbits.LATA0 = 1; } while(0)
#define LCD_CS_SetLow()             do { LATAbits.LATA0 = 0; } while(0)
#define LCD_CS_Toggle()             do { LATAbits.LATA0 = ~LATAbits.LATA0; } while(0)
#define LCD_CS_GetValue()           PORTAbits.RA0
#define LCD_CS_SetDigitalInput()    do { TRISAbits.TRISA0 = 1; } while(0)
#define LCD_CS_SetDigitalOutput()   do { TRISAbits.TRISA0 = 0; } while(0)
#define LCD_CS_SetPullup()          do { WPUAbits.WPUA0 = 1; } while(0)
#define LCD_CS_ResetPullup()        do { WPUAbits.WPUA0 = 0; } while(0)
#define LCD_CS_SetPushPull()        do { ODCONAbits.ODCA0 = 0; } while(0)
#define LCD_CS_SetOpenDrain()       do { ODCONAbits.ODCA0 = 1; } while(0)
#define LCD_CS_SetAnalogMode()      do { ANSELAbits.ANSELA0 = 1; } while(0)
#define LCD_CS_SetDigitalMode()     do { ANSELAbits.ANSELA0 = 0; } while(0)

// get/set LCD_RESET aliases
#define LCD_RESET_TRIS                 TRISAbits.TRISA1
#define LCD_RESET_LAT                  LATAbits.LATA1
#define LCD_RESET_PORT                 PORTAbits.RA1
#define LCD_RESET_WPU                  WPUAbits.WPUA1
#define LCD_RESET_OD                   ODCONAbits.ODCA1
#define LCD_RESET_ANS                  ANSELAbits.ANSELA1
#define LCD_RESET_SetHigh()            do { LATAbits.LATA1 = 1; } while(0)
#define LCD_RESET_SetLow()             do { LATAbits.LATA1 = 0; } while(0)
#define LCD_RESET_Toggle()             do { LATAbits.LATA1 = ~LATAbits.LATA1; } while(0)
#define LCD_RESET_GetValue()           PORTAbits.RA1
#define LCD_RESET_SetDigitalInput()    do { TRISAbits.TRISA1 = 1; } while(0)
#define LCD_RESET_SetDigitalOutput()   do { TRISAbits.TRISA1 = 0; } while(0)
#define LCD_RESET_SetPullup()          do { WPUAbits.WPUA1 = 1; } while(0)
#define LCD_RESET_ResetPullup()        do { WPUAbits.WPUA1 = 0; } while(0)
#define LCD_RESET_SetPushPull()        do { ODCONAbits.ODCA1 = 0; } while(0)
#define LCD_RESET_SetOpenDrain()       do { ODCONAbits.ODCA1 = 1; } while(0)
#define LCD_RESET_SetAnalogMode()      do { ANSELAbits.ANSELA1 = 1; } while(0)
#define LCD_RESET_SetDigitalMode()     do { ANSELAbits.ANSELA1 = 0; } while(0)

// get/set LCD_DCRS aliases
#define LCD_DCRS_TRIS                 TRISAbits.TRISA2
#define LCD_DCRS_LAT                  LATAbits.LATA2
#define LCD_DCRS_PORT                 PORTAbits.RA2
#define LCD_DCRS_WPU                  WPUAbits.WPUA2
#define LCD_DCRS_OD                   ODCONAbits.ODCA2
#define LCD_DCRS_ANS                  ANSELAbits.ANSELA2
#define LCD_DCRS_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define LCD_DCRS_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define LCD_DCRS_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define LCD_DCRS_GetValue()           PORTAbits.RA2
#define LCD_DCRS_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define LCD_DCRS_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define LCD_DCRS_SetPullup()          do { WPUAbits.WPUA2 = 1; } while(0)
#define LCD_DCRS_ResetPullup()        do { WPUAbits.WPUA2 = 0; } while(0)
#define LCD_DCRS_SetPushPull()        do { ODCONAbits.ODCA2 = 0; } while(0)
#define LCD_DCRS_SetOpenDrain()       do { ODCONAbits.ODCA2 = 1; } while(0)
#define LCD_DCRS_SetAnalogMode()      do { ANSELAbits.ANSELA2 = 1; } while(0)
#define LCD_DCRS_SetDigitalMode()     do { ANSELAbits.ANSELA2 = 0; } while(0)

// get/set PhotoDiode aliases
#define PhotoDiode_TRIS                 TRISAbits.TRISA3
#define PhotoDiode_LAT                  LATAbits.LATA3
#define PhotoDiode_PORT                 PORTAbits.RA3
#define PhotoDiode_WPU                  WPUAbits.WPUA3
#define PhotoDiode_OD                   ODCONAbits.ODCA3
#define PhotoDiode_ANS                  ANSELAbits.ANSELA3
#define PhotoDiode_SetHigh()            do { LATAbits.LATA3 = 1; } while(0)
#define PhotoDiode_SetLow()             do { LATAbits.LATA3 = 0; } while(0)
#define PhotoDiode_Toggle()             do { LATAbits.LATA3 = ~LATAbits.LATA3; } while(0)
#define PhotoDiode_GetValue()           PORTAbits.RA3
#define PhotoDiode_SetDigitalInput()    do { TRISAbits.TRISA3 = 1; } while(0)
#define PhotoDiode_SetDigitalOutput()   do { TRISAbits.TRISA3 = 0; } while(0)
#define PhotoDiode_SetPullup()          do { WPUAbits.WPUA3 = 1; } while(0)
#define PhotoDiode_ResetPullup()        do { WPUAbits.WPUA3 = 0; } while(0)
#define PhotoDiode_SetPushPull()        do { ODCONAbits.ODCA3 = 0; } while(0)
#define PhotoDiode_SetOpenDrain()       do { ODCONAbits.ODCA3 = 1; } while(0)
#define PhotoDiode_SetAnalogMode()      do { ANSELAbits.ANSELA3 = 1; } while(0)
#define PhotoDiode_SetDigitalMode()     do { ANSELAbits.ANSELA3 = 0; } while(0)

// get/set T_IRQ aliases
#define T_IRQ_TRIS                 TRISAbits.TRISA4
#define T_IRQ_LAT                  LATAbits.LATA4
#define T_IRQ_PORT                 PORTAbits.RA4
#define T_IRQ_WPU                  WPUAbits.WPUA4
#define T_IRQ_OD                   ODCONAbits.ODCA4
#define T_IRQ_ANS                  ANSELAbits.ANSELA4
#define T_IRQ_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define T_IRQ_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define T_IRQ_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define T_IRQ_GetValue()           PORTAbits.RA4
#define T_IRQ_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define T_IRQ_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)
#define T_IRQ_SetPullup()          do { WPUAbits.WPUA4 = 1; } while(0)
#define T_IRQ_ResetPullup()        do { WPUAbits.WPUA4 = 0; } while(0)
#define T_IRQ_SetPushPull()        do { ODCONAbits.ODCA4 = 0; } while(0)
#define T_IRQ_SetOpenDrain()       do { ODCONAbits.ODCA4 = 1; } while(0)
#define T_IRQ_SetAnalogMode()      do { ANSELAbits.ANSELA4 = 1; } while(0)
#define T_IRQ_SetDigitalMode()     do { ANSELAbits.ANSELA4 = 0; } while(0)

// get/set T_CS aliases
#define T_CS_TRIS                 TRISAbits.TRISA5
#define T_CS_LAT                  LATAbits.LATA5
#define T_CS_PORT                 PORTAbits.RA5
#define T_CS_WPU                  WPUAbits.WPUA5
#define T_CS_OD                   ODCONAbits.ODCA5
#define T_CS_ANS                  ANSELAbits.ANSELA5
#define T_CS_SetHigh()            do { LATAbits.LATA5 = 1; } while(0)
#define T_CS_SetLow()             do { LATAbits.LATA5 = 0; } while(0)
#define T_CS_Toggle()             do { LATAbits.LATA5 = ~LATAbits.LATA5; } while(0)
#define T_CS_GetValue()           PORTAbits.RA5
#define T_CS_SetDigitalInput()    do { TRISAbits.TRISA5 = 1; } while(0)
#define T_CS_SetDigitalOutput()   do { TRISAbits.TRISA5 = 0; } while(0)
#define T_CS_SetPullup()          do { WPUAbits.WPUA5 = 1; } while(0)
#define T_CS_ResetPullup()        do { WPUAbits.WPUA5 = 0; } while(0)
#define T_CS_SetPushPull()        do { ODCONAbits.ODCA5 = 0; } while(0)
#define T_CS_SetOpenDrain()       do { ODCONAbits.ODCA5 = 1; } while(0)
#define T_CS_SetAnalogMode()      do { ANSELAbits.ANSELA5 = 1; } while(0)
#define T_CS_SetDigitalMode()     do { ANSELAbits.ANSELA5 = 0; } while(0)

// get/set SDCard_CS aliases
#define SDCard_CS_TRIS                 TRISAbits.TRISA6
#define SDCard_CS_LAT                  LATAbits.LATA6
#define SDCard_CS_PORT                 PORTAbits.RA6
#define SDCard_CS_WPU                  WPUAbits.WPUA6
#define SDCard_CS_OD                   ODCONAbits.ODCA6
#define SDCard_CS_ANS                  ANSELAbits.ANSELA6
#define SDCard_CS_SetHigh()            do { LATAbits.LATA6 = 1; } while(0)
#define SDCard_CS_SetLow()             do { LATAbits.LATA6 = 0; } while(0)
#define SDCard_CS_Toggle()             do { LATAbits.LATA6 = ~LATAbits.LATA6; } while(0)
#define SDCard_CS_GetValue()           PORTAbits.RA6
#define SDCard_CS_SetDigitalInput()    do { TRISAbits.TRISA6 = 1; } while(0)
#define SDCard_CS_SetDigitalOutput()   do { TRISAbits.TRISA6 = 0; } while(0)
#define SDCard_CS_SetPullup()          do { WPUAbits.WPUA6 = 1; } while(0)
#define SDCard_CS_ResetPullup()        do { WPUAbits.WPUA6 = 0; } while(0)
#define SDCard_CS_SetPushPull()        do { ODCONAbits.ODCA6 = 0; } while(0)
#define SDCard_CS_SetOpenDrain()       do { ODCONAbits.ODCA6 = 1; } while(0)
#define SDCard_CS_SetAnalogMode()      do { ANSELAbits.ANSELA6 = 1; } while(0)
#define SDCard_CS_SetDigitalMode()     do { ANSELAbits.ANSELA6 = 0; } while(0)

// get/set ALMSW aliases
#define ALMSW_TRIS                 TRISAbits.TRISA7
#define ALMSW_LAT                  LATAbits.LATA7
#define ALMSW_PORT                 PORTAbits.RA7
#define ALMSW_WPU                  WPUAbits.WPUA7
#define ALMSW_OD                   ODCONAbits.ODCA7
#define ALMSW_ANS                  ANSELAbits.ANSELA7
#define ALMSW_SetHigh()            do { LATAbits.LATA7 = 1; } while(0)
#define ALMSW_SetLow()             do { LATAbits.LATA7 = 0; } while(0)
#define ALMSW_Toggle()             do { LATAbits.LATA7 = ~LATAbits.LATA7; } while(0)
#define ALMSW_GetValue()           PORTAbits.RA7
#define ALMSW_SetDigitalInput()    do { TRISAbits.TRISA7 = 1; } while(0)
#define ALMSW_SetDigitalOutput()   do { TRISAbits.TRISA7 = 0; } while(0)
#define ALMSW_SetPullup()          do { WPUAbits.WPUA7 = 1; } while(0)
#define ALMSW_ResetPullup()        do { WPUAbits.WPUA7 = 0; } while(0)
#define ALMSW_SetPushPull()        do { ODCONAbits.ODCA7 = 0; } while(0)
#define ALMSW_SetOpenDrain()       do { ODCONAbits.ODCA7 = 1; } while(0)
#define ALMSW_SetAnalogMode()      do { ANSELAbits.ANSELA7 = 1; } while(0)
#define ALMSW_SetDigitalMode()     do { ANSELAbits.ANSELA7 = 0; } while(0)

// get/set RB0 procedures
#define RB0_SetHigh()            do { LATBbits.LATB0 = 1; } while(0)
#define RB0_SetLow()             do { LATBbits.LATB0 = 0; } while(0)
#define RB0_Toggle()             do { LATBbits.LATB0 = ~LATBbits.LATB0; } while(0)
#define RB0_GetValue()              PORTBbits.RB0
#define RB0_SetDigitalInput()    do { TRISBbits.TRISB0 = 1; } while(0)
#define RB0_SetDigitalOutput()   do { TRISBbits.TRISB0 = 0; } while(0)
#define RB0_SetPullup()             do { WPUBbits.WPUB0 = 1; } while(0)
#define RB0_ResetPullup()           do { WPUBbits.WPUB0 = 0; } while(0)
#define RB0_SetAnalogMode()         do { ANSELBbits.ANSELB0 = 1; } while(0)
#define RB0_SetDigitalMode()        do { ANSELBbits.ANSELB0 = 0; } while(0)

// get/set RB1 procedures
#define RB1_SetHigh()            do { LATBbits.LATB1 = 1; } while(0)
#define RB1_SetLow()             do { LATBbits.LATB1 = 0; } while(0)
#define RB1_Toggle()             do { LATBbits.LATB1 = ~LATBbits.LATB1; } while(0)
#define RB1_GetValue()              PORTBbits.RB1
#define RB1_SetDigitalInput()    do { TRISBbits.TRISB1 = 1; } while(0)
#define RB1_SetDigitalOutput()   do { TRISBbits.TRISB1 = 0; } while(0)
#define RB1_SetPullup()             do { WPUBbits.WPUB1 = 1; } while(0)
#define RB1_ResetPullup()           do { WPUBbits.WPUB1 = 0; } while(0)
#define RB1_SetAnalogMode()         do { ANSELBbits.ANSELB1 = 1; } while(0)
#define RB1_SetDigitalMode()        do { ANSELBbits.ANSELB1 = 0; } while(0)

// get/set RB2 procedures
#define RB2_SetHigh()            do { LATBbits.LATB2 = 1; } while(0)
#define RB2_SetLow()             do { LATBbits.LATB2 = 0; } while(0)
#define RB2_Toggle()             do { LATBbits.LATB2 = ~LATBbits.LATB2; } while(0)
#define RB2_GetValue()              PORTBbits.RB2
#define RB2_SetDigitalInput()    do { TRISBbits.TRISB2 = 1; } while(0)
#define RB2_SetDigitalOutput()   do { TRISBbits.TRISB2 = 0; } while(0)
#define RB2_SetPullup()             do { WPUBbits.WPUB2 = 1; } while(0)
#define RB2_ResetPullup()           do { WPUBbits.WPUB2 = 0; } while(0)
#define RB2_SetAnalogMode()         do { ANSELBbits.ANSELB2 = 1; } while(0)
#define RB2_SetDigitalMode()        do { ANSELBbits.ANSELB2 = 0; } while(0)

// get/set RB3 procedures
#define RB3_SetHigh()            do { LATBbits.LATB3 = 1; } while(0)
#define RB3_SetLow()             do { LATBbits.LATB3 = 0; } while(0)
#define RB3_Toggle()             do { LATBbits.LATB3 = ~LATBbits.LATB3; } while(0)
#define RB3_GetValue()              PORTBbits.RB3
#define RB3_SetDigitalInput()    do { TRISBbits.TRISB3 = 1; } while(0)
#define RB3_SetDigitalOutput()   do { TRISBbits.TRISB3 = 0; } while(0)
#define RB3_SetPullup()             do { WPUBbits.WPUB3 = 1; } while(0)
#define RB3_ResetPullup()           do { WPUBbits.WPUB3 = 0; } while(0)
#define RB3_SetAnalogMode()         do { ANSELBbits.ANSELB3 = 1; } while(0)
#define RB3_SetDigitalMode()        do { ANSELBbits.ANSELB3 = 0; } while(0)

// get/set RTC_INTB aliases
#define RTC_INTB_TRIS                 TRISBbits.TRISB4
#define RTC_INTB_LAT                  LATBbits.LATB4
#define RTC_INTB_PORT                 PORTBbits.RB4
#define RTC_INTB_WPU                  WPUBbits.WPUB4
#define RTC_INTB_OD                   ODCONBbits.ODCB4
#define RTC_INTB_ANS                  ANSELBbits.ANSELB4
#define RTC_INTB_SetHigh()            do { LATBbits.LATB4 = 1; } while(0)
#define RTC_INTB_SetLow()             do { LATBbits.LATB4 = 0; } while(0)
#define RTC_INTB_Toggle()             do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define RTC_INTB_GetValue()           PORTBbits.RB4
#define RTC_INTB_SetDigitalInput()    do { TRISBbits.TRISB4 = 1; } while(0)
#define RTC_INTB_SetDigitalOutput()   do { TRISBbits.TRISB4 = 0; } while(0)
#define RTC_INTB_SetPullup()          do { WPUBbits.WPUB4 = 1; } while(0)
#define RTC_INTB_ResetPullup()        do { WPUBbits.WPUB4 = 0; } while(0)
#define RTC_INTB_SetPushPull()        do { ODCONBbits.ODCB4 = 0; } while(0)
#define RTC_INTB_SetOpenDrain()       do { ODCONBbits.ODCB4 = 1; } while(0)
#define RTC_INTB_SetAnalogMode()      do { ANSELBbits.ANSELB4 = 1; } while(0)
#define RTC_INTB_SetDigitalMode()     do { ANSELBbits.ANSELB4 = 0; } while(0)

// get/set G1PPS aliases
#define G1PPS_TRIS                 TRISBbits.TRISB5
#define G1PPS_LAT                  LATBbits.LATB5
#define G1PPS_PORT                 PORTBbits.RB5
#define G1PPS_WPU                  WPUBbits.WPUB5
#define G1PPS_OD                   ODCONBbits.ODCB5
#define G1PPS_ANS                  ANSELBbits.ANSELB5
#define G1PPS_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define G1PPS_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define G1PPS_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define G1PPS_GetValue()           PORTBbits.RB5
#define G1PPS_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define G1PPS_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define G1PPS_SetPullup()          do { WPUBbits.WPUB5 = 1; } while(0)
#define G1PPS_ResetPullup()        do { WPUBbits.WPUB5 = 0; } while(0)
#define G1PPS_SetPushPull()        do { ODCONBbits.ODCB5 = 0; } while(0)
#define G1PPS_SetOpenDrain()       do { ODCONBbits.ODCB5 = 1; } while(0)
#define G1PPS_SetAnalogMode()      do { ANSELBbits.ANSELB5 = 1; } while(0)
#define G1PPS_SetDigitalMode()     do { ANSELBbits.ANSELB5 = 0; } while(0)

// get/set ROT_B aliases
#define ROT_B_TRIS                 TRISCbits.TRISC0
#define ROT_B_LAT                  LATCbits.LATC0
#define ROT_B_PORT                 PORTCbits.RC0
#define ROT_B_WPU                  WPUCbits.WPUC0
#define ROT_B_OD                   ODCONCbits.ODCC0
#define ROT_B_ANS                  ANSELCbits.ANSELC0
#define ROT_B_SetHigh()            do { LATCbits.LATC0 = 1; } while(0)
#define ROT_B_SetLow()             do { LATCbits.LATC0 = 0; } while(0)
#define ROT_B_Toggle()             do { LATCbits.LATC0 = ~LATCbits.LATC0; } while(0)
#define ROT_B_GetValue()           PORTCbits.RC0
#define ROT_B_SetDigitalInput()    do { TRISCbits.TRISC0 = 1; } while(0)
#define ROT_B_SetDigitalOutput()   do { TRISCbits.TRISC0 = 0; } while(0)
#define ROT_B_SetPullup()          do { WPUCbits.WPUC0 = 1; } while(0)
#define ROT_B_ResetPullup()        do { WPUCbits.WPUC0 = 0; } while(0)
#define ROT_B_SetPushPull()        do { ODCONCbits.ODCC0 = 0; } while(0)
#define ROT_B_SetOpenDrain()       do { ODCONCbits.ODCC0 = 1; } while(0)
#define ROT_B_SetAnalogMode()      do { ANSELCbits.ANSELC0 = 1; } while(0)
#define ROT_B_SetDigitalMode()     do { ANSELCbits.ANSELC0 = 0; } while(0)

// get/set ROT_A aliases
#define ROT_A_TRIS                 TRISCbits.TRISC1
#define ROT_A_LAT                  LATCbits.LATC1
#define ROT_A_PORT                 PORTCbits.RC1
#define ROT_A_WPU                  WPUCbits.WPUC1
#define ROT_A_OD                   ODCONCbits.ODCC1
#define ROT_A_ANS                  ANSELCbits.ANSELC1
#define ROT_A_SetHigh()            do { LATCbits.LATC1 = 1; } while(0)
#define ROT_A_SetLow()             do { LATCbits.LATC1 = 0; } while(0)
#define ROT_A_Toggle()             do { LATCbits.LATC1 = ~LATCbits.LATC1; } while(0)
#define ROT_A_GetValue()           PORTCbits.RC1
#define ROT_A_SetDigitalInput()    do { TRISCbits.TRISC1 = 1; } while(0)
#define ROT_A_SetDigitalOutput()   do { TRISCbits.TRISC1 = 0; } while(0)
#define ROT_A_SetPullup()          do { WPUCbits.WPUC1 = 1; } while(0)
#define ROT_A_ResetPullup()        do { WPUCbits.WPUC1 = 0; } while(0)
#define ROT_A_SetPushPull()        do { ODCONCbits.ODCC1 = 0; } while(0)
#define ROT_A_SetOpenDrain()       do { ODCONCbits.ODCC1 = 1; } while(0)
#define ROT_A_SetAnalogMode()      do { ANSELCbits.ANSELC1 = 1; } while(0)
#define ROT_A_SetDigitalMode()     do { ANSELCbits.ANSELC1 = 0; } while(0)

// get/set RC2 procedures
#define RC2_SetHigh()            do { LATCbits.LATC2 = 1; } while(0)
#define RC2_SetLow()             do { LATCbits.LATC2 = 0; } while(0)
#define RC2_Toggle()             do { LATCbits.LATC2 = ~LATCbits.LATC2; } while(0)
#define RC2_GetValue()              PORTCbits.RC2
#define RC2_SetDigitalInput()    do { TRISCbits.TRISC2 = 1; } while(0)
#define RC2_SetDigitalOutput()   do { TRISCbits.TRISC2 = 0; } while(0)
#define RC2_SetPullup()             do { WPUCbits.WPUC2 = 1; } while(0)
#define RC2_ResetPullup()           do { WPUCbits.WPUC2 = 0; } while(0)
#define RC2_SetAnalogMode()         do { ANSELCbits.ANSELC2 = 1; } while(0)
#define RC2_SetDigitalMode()        do { ANSELCbits.ANSELC2 = 0; } while(0)

// get/set RC3 procedures
#define RC3_SetHigh()            do { LATCbits.LATC3 = 1; } while(0)
#define RC3_SetLow()             do { LATCbits.LATC3 = 0; } while(0)
#define RC3_Toggle()             do { LATCbits.LATC3 = ~LATCbits.LATC3; } while(0)
#define RC3_GetValue()              PORTCbits.RC3
#define RC3_SetDigitalInput()    do { TRISCbits.TRISC3 = 1; } while(0)
#define RC3_SetDigitalOutput()   do { TRISCbits.TRISC3 = 0; } while(0)
#define RC3_SetPullup()             do { WPUCbits.WPUC3 = 1; } while(0)
#define RC3_ResetPullup()           do { WPUCbits.WPUC3 = 0; } while(0)
#define RC3_SetAnalogMode()         do { ANSELCbits.ANSELC3 = 1; } while(0)
#define RC3_SetDigitalMode()        do { ANSELCbits.ANSELC3 = 0; } while(0)

// get/set RC4 procedures
#define RC4_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define RC4_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define RC4_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define RC4_GetValue()              PORTCbits.RC4
#define RC4_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define RC4_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)
#define RC4_SetPullup()             do { WPUCbits.WPUC4 = 1; } while(0)
#define RC4_ResetPullup()           do { WPUCbits.WPUC4 = 0; } while(0)
#define RC4_SetAnalogMode()         do { ANSELCbits.ANSELC4 = 1; } while(0)
#define RC4_SetDigitalMode()        do { ANSELCbits.ANSELC4 = 0; } while(0)

// get/set RC5 procedures
#define RC5_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define RC5_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define RC5_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define RC5_GetValue()              PORTCbits.RC5
#define RC5_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define RC5_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define RC5_SetPullup()             do { WPUCbits.WPUC5 = 1; } while(0)
#define RC5_ResetPullup()           do { WPUCbits.WPUC5 = 0; } while(0)
#define RC5_SetAnalogMode()         do { ANSELCbits.ANSELC5 = 1; } while(0)
#define RC5_SetDigitalMode()        do { ANSELCbits.ANSELC5 = 0; } while(0)

// get/set RC7 procedures
#define RC7_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define RC7_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define RC7_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define RC7_GetValue()              PORTCbits.RC7
#define RC7_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define RC7_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define RC7_SetPullup()             do { WPUCbits.WPUC7 = 1; } while(0)
#define RC7_ResetPullup()           do { WPUCbits.WPUC7 = 0; } while(0)
#define RC7_SetAnalogMode()         do { ANSELCbits.ANSELC7 = 1; } while(0)
#define RC7_SetDigitalMode()        do { ANSELCbits.ANSELC7 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);


/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handler for the IOCBF4 pin functionality
 * @Example
    IOCBF4_ISR();
 */
void IOCBF4_ISR(void);

/**
  @Summary
    Interrupt Handler Setter for IOCBF4 pin interrupt-on-change functionality

  @Description
    Allows selecting an interrupt handler for IOCBF4 at application runtime
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    InterruptHandler function pointer.

  @Example
    PIN_MANAGER_Initialize();
    IOCBF4_SetInterruptHandler(MyInterruptHandler);

*/
void IOCBF4_SetInterruptHandler(void (* InterruptHandler)(void));

/**
  @Summary
    Dynamic Interrupt Handler for IOCBF4 pin

  @Description
    This is a dynamic interrupt handler to be used together with the IOCBF4_SetInterruptHandler() method.
    This handler is called every time the IOCBF4 ISR is executed and allows any function to be registered at runtime.
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCBF4_SetInterruptHandler(IOCBF4_InterruptHandler);

*/
extern void (*IOCBF4_InterruptHandler)(void);

/**
  @Summary
    Default Interrupt Handler for IOCBF4 pin

  @Description
    This is a predefined interrupt handler to be used together with the IOCBF4_SetInterruptHandler() method.
    This handler is called every time the IOCBF4 ISR is executed. 
    
  @Preconditions
    Pin Manager intializer called

  @Returns
    None.

  @Param
    None.

  @Example
    PIN_MANAGER_Initialize();
    IOCBF4_SetInterruptHandler(IOCBF4_DefaultInterruptHandler);

*/
void IOCBF4_DefaultInterruptHandler(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/