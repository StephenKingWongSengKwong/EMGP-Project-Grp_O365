
#include <xc.h>
#define _XTAL_FREQ 8000000


//LCD connections
#define RS PORTDbits.RD2
#define EN PORTDbits.RD3
#define D4 PORTDbits.RD4
#define D5 PORTDbits.RD5
#define D6 PORTDbits.RD6
#define D7 PORTDbits.RD7

#include "lcd.h"
#include "config.h"
#include <pic18f87j11.h>

void showVoltageOnDisplay (float adc2);
void configureADC (void);

void main()
{
    //****************************set up LCD  **********************//
    TRISD=0x00;
    PORTD=0;
    PORTB=0x00;
    TRISB=0x00;
    
    Lcd_Init();
    Lcd_Clear();   
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("    Voltage");
    //****************************End of set up LCD  **********************//
 
    configureADC();
   
    /*
     * Next part below starts the AD conversion process on channel AN0,
     * waits for conversion to finish
     * calculates the actual analog voltage,
     * displays it on a LCD for verification
   */
 while(1) {              
                        __delay_ms(20);
                        float adc=0.0;
                        ADCON0bits.GO_DONE = 1; // Start Conversion               
                        while (ADCON0bits.GO_DONE == 1){ 
                                    // wait
                        }                               
                        adc=ADRESH*256+ADRESL; //ADRESL = lower bits   //ADRESH = higher bits
                        adc = adc*0.00488;
                        if (adc > 3){
                            LATB = 0xFF;
                        } else {
                            LATB = 0x00;
                        }
                        showVoltageOnDisplay(adc);
    }	
}


void configureADC (void){
    //*************************configure port A for ADC
    TRISA=0xFF;  //port A as input
    ADCON0bits.VCFG1 =0;  // use internal voltage VSS and VDD as reference
    ADCON0bits.VCFG0 =0;
    
    
    ADCON0bits.CHS0=0;  //select channel AN0
    ADCON0bits.CHS1=0;
    ADCON0bits.CHS2=0;
    ADCON0bits.CHS3=0;
    
    ADCON1bits.ADFM=1;  //right justification of result
    
    ADCON1bits.ACQT0=0; //set AD acquisition time of 16TAD
    ADCON1bits.ACQT1=1;
    ADCON1bits.ACQT2=1;
   
    
    ADCON1bits.ADCS0=0; //select AD clock, FOSC/4  // this is needed for PIC18F87J11
    ADCON1bits.ADCS1=0;
    ADCON1bits.ADCS2=1;
 

    ADCON0bits.ADON =1; //turn on ADC
}


/*
     * This code belows only converts analog value to characters so that it 
 * can be displayed on a LCD.
 * number 0 and character 0 are not the same thing. Character 0 is obtained
 * by adding 48 to 0. So character 1 will be created by adding 48 to number 1,
   */

void showVoltageOnDisplay (float adc2) { 
                    
                        int te =0;
                        char lcdc[] = "00.00";
                        te  = adc2/10;
                         if (te>0) {
                            lcdc[0] = te + 48;      // convert it to a character
                            adc2 = adc2 - (te)*10;
                         }  else { 
                             lcdc[0] = 48; 
                         }
                         te  = adc2;
                          if (te>0) {
                            lcdc[1] = te + 48;
                            adc2 = adc2 - (te);
                         } else {
                             lcdc[1] = 48; 
                         }
                         te  = adc2*10;
                          if (te>0) {
                            lcdc[3] = te + 48;
                            adc2 = adc2 - (te);
                         } else {
                             lcdc[3] = 48; 
                         }
                         te  = adc2*100;
                          if (te>0) {
                            lcdc[4] = te + 48;
                            //adc2 = adc2 - (te)/10;
                         } else {
                             lcdc[4] = 48; 
                         }
                          
                          Lcd_Set_Cursor(2,5);
                          Lcd_Write_String(lcdc);
                          Lcd_Write_Char(' '); //ASCII code for degree sign
                          Lcd_Write_Char('V');    
                          return;
}