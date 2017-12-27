/*
 * 
 * a. name of students: WSK
                        Richard 
                        YYK
                        TTY 
 * b. student ID(following the same row):   ()
 *                                          () 
from CU                                     ()
                                            ()
 * c. title " assignment 1"
 * d. date: 17 jan 2017
*/

#include <xc.h>
#include <p18f87j11.h>
#include <plib/adc.h>
#include <plib/delays.h>
#include <stdlib.h>
#pragma   FOSC = HSHP
#pragma config XINST = OFF
#define _XTAL_FREQ 10000000

void fowardFullStep() {//rd0-rd3 left, rd4-rd7 right
    __delay_ms(3);
    LATD = 0x9C;
    __delay_ms(3);
    LATD = 0x36;
    __delay_ms(3);
    LATD = 0x63;
    __delay_ms(3);
    LATD = 0xC9;
}

void backwardFullStep() {
    __delay_ms(3);
    LATD = 0xC9;
    __delay_ms(3);
    LATD = 0x63;
    __delay_ms(3);
    LATD = 0x36;
    __delay_ms(3);
    LATD = 0x9C;
}

void ftest() {
    __delay_ms(30);
    LATD = 0xFF;
    __delay_ms(30);
    LATD = 0x00;
}

void btest() {
    __delay_ms(30);
    LATD = 0xCC;
    __delay_ms(30);
    LATD = 0x00;
}

void fowardHalfStep() {
    LATD = 0x98;
    __delay_ms(3);
    LATD = 0x1C;
    __delay_ms(3);
    LATD = 0x34;
    __delay_ms(3);
    LATD = 0x26;
    __delay_ms(3);
    LATD = 0x62;
    __delay_ms(3);
    LATD = 0x43;
    __delay_ms(3);
    LATD = 0xC1;
    __delay_ms(3);
    LATD = 0x89;
    __delay_ms(3);
}

void backwardHalfStep() {
    LATD = 0x89;
    __delay_ms(3);
    LATD = 0xC1;
    __delay_ms(3);
    LATD = 0x43;
    __delay_ms(3);
    LATD = 0x62;
    __delay_ms(3);
    LATD = 0x26;
    __delay_ms(3);
    LATD = 0x34;
    __delay_ms(3);
    LATD = 0x1C;
    __delay_ms(3);
    LATD = 0x98;
    __delay_ms(3);
}

int proximityADC(long Vin, long mV, int state, int mVinitial) {
    /*state 1: on,forward
     *state 2: backward
     state 1; off */
    SelChanConvADC(ADC_CH1); // channel 1
    ConvertADC();
    while (BusyADC())
        Vin = ReadADC();
    mV = (Vin * 3300) / 1024;
    if (state == 1) {
        fowardFullStep();

        if (mV <= 2950) {
            return 1;
        } else if (mV > 2950) {
            return 2;
        }
    } else if (state == 2) {
        backwardFullStep();
        if (mV > mVinitial) {
            return 2;
        }
        if (mV <= mVinitial) {
            return -1;
        }
    }
    return-1;
}

void testADC(long Vin, long mV) {
    SelChanConvADC(ADC_CH1); // channel 1
    ConvertADC();
    while (BusyADC())
        Vin = ReadADC();
    mV = (Vin * 3300) / 1024;
    if (mV <= 1100) {
        LATD = 0b00000001;
    } else if (mV > 1100 && mV <= 1400) {
        LATD = 0b00000010;
    } else if (mV > 1400 && mV <= 1700) {
        LATD = 0b00000100;
    } else if (mV > 1700 && mV <= 2000) {
        LATD = 0b00001000;
    } else if (mV > 2000 && mV <= 2300) {
        LATD = 0b00010000;
    } else if (mV > 2300 && mV <= 2600) {
        LATD = 0b00100000;
    } else if (mV > 2600 && mV <= 2900) {
        LATD = 0b01000000;
    } else if (mV > 2900 && mV <= 3300) {
        LATD = 0b10000000;
    }
    __delay_ms(25);
}

int testDigital(int flag) {
    if (PORTBbits.RB0 == 1 && flag == 0) {
        __delay_ms(100);
        LATD = 0xFF;
        __delay_ms(100);
        LATD = 0xCC;
        return 1;
    } else if (PORTBbits.RB0 == 0 && flag == 1) {
        __delay_ms(100);
        LATD = 0x00;
        __delay_ms(100);
        LATD = 0x11;
        return 0;
    }
}

int opticalDigitalFlag(int flag) {
    if (PORTBbits.RB0 == 1 && flag == 1) {
        LATC = 0xFF;
        return 0;
    } else if (PORTBbits.RB0 == 0 && flag == 0) {
        LATC = 0x00;
        return 1;
    }
    return flag;
}

int opticalDigital() {
    if (PORTBbits.RB0 == 1) {
        LATC = 0xFF;
    } else if (PORTBbits.RB0 == 0) {
        LATC = 0x00;
    }
}

void main() {
    unsigned long Vin, mV, Vinitial, mVinitial;
    LATD = 0x00;
    PORTA = 0;
    LATA = 0;
    TRISA = 0x02; //port RA1/AN1
    TRISB = 0x01; //RB0
    TRISC = 0;
    TRISD = 0;
    int optflag = 0;
    int proxystate = 1;
    int proxyref = 1;
    OpenADC(ADC_FOSC_32 & ADC_RIGHT_JUST & ADC_12_TAD,
            ADC_CH1 & ADC_INT_OFF & ADC_REF_VDD_VSS,
            ADC_5ANA & 0xf);
    //initial voltage
    SelChanConvADC(ADC_CH1); // channel 1
    ConvertADC();
    while (BusyADC())
        Vinitial = ReadADC();
    mVinitial = (Vinitial * 3300) / 1024;
    if (mVinitial < 900) {
        mVinitial = 900;
    }
    //
    while (1) {
        proxystate = proximityADC(Vin, mV, proxystate, mVinitial);
        opticalDigital();
    }
}