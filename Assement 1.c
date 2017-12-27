#include <xc.h>
#include "configuration.h"
#include <pic18f4550.h>
#define stepper p1;

void delay(void);

void main(void)
{
    TRISD = 0x00;
    PORTD = 0x00;
    PORTD=255;
  
    while(1) 
    {
              
        stepper = 0x0C;
		delay();
		stepper = 0x06;
		delay();
		stepper = 0x03;
		delay();
		stepper = 0x09;
		delay();
 
    }
    return;
    }

void delay()
{
	unsigned char i,j,k;
	for(i=0;i<6;i++) 
    {
		for(j=0;j<255;j++)
			for(k=0;k<255;k++);
	}
}