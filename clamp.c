#define F_CPU 2000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd4bit.h"
#include <math.h>
#include <stdlib.h>

//ADC  values and functions initialization
int adc_read(int ch);
int adc;
char buf[10]; 
float am=0;

//Ammeter Functions
// initialize adc
void adc_init()
{
	DDRA = 0x0;
    // AREF = AVcc
    ADMUX = (1<<REFS0);
  
    // ADC Enable and prescaler of 128
    // 8000000/128 = 62500
    ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}
  
// read adc value
int adc_read(int ch)
{
	int Ain,AinLow;
	
    // select the corresponding channel 0~7
    // ANDing with '7' will always keep the value
    // of 'ch' between 0 and 7
    ch &= 0b00000111;  // AND operation with 7
    ADMUX = (ADMUX & 0xF8)|ch;     // clears the bottom 3 bits before ORing
  
    // start single conversion
    // write '1' to ADSC
    ADCSRA |= (1<<ADSC);
  
    // wait for conversion to complete
    // ADSC becomes '0' again
    // till then, run loop continuously
    while((ADCSRA&(1<<ADIF))==0);
	
	//_delay_us(10);
	AinLow = (int)ADCL;		/* Read lower byte*/
	Ain = (int)ADCH*256;		/* Read higher 2 bits and Multiply with weight */
	Ain = Ain + AinLow;	
	
	return(Ain);			/* Return digital value*/
    //return (ADCW);
}





void main(void)
{
// Declare your local variables here
int adc_int[41];
int i=0,j=0,max=0;
unsigned char arr[5];
int battery_positive;
int battery_negative;

float bat1;
float bat2;
int disp1;
int disp2;

 // Input/Output Ports initialization

DDRC = 0x00;
DDRB = 0xff;
DDRD = 0xff;

//__LCD Initialization____
        LCDInit(0);
		LCDClear();
		//LCDWriteStringXY(0,0,"B1:");
		//LCDWriteStringXY(6,0,"%");
		//LCDWriteStringXY(9,0,"B2:");
		//LCDWriteStringXY(15,0,"%");

// initialize ADC
adc_init();
       
while(1)
{  

battery_positive = adc_read(1);  //PA1
battery_negative = adc_read(2);   //PA2

bat1 = (float)(battery_positive*0.09765625);
disp1 = (int)bat1;

bat2 = (float)(battery_negative*0.09765625);
disp2 = (int)bat2;

LCDWriteStringXY(0,0,"B1:");
LCDGotoXY(3,0);
LCDWriteInt(disp1,2);
LCDWriteStringXY(5,0,"%");

LCDWriteStringXY(10,0,"B2:");
LCDGotoXY(13,0);
LCDWriteInt(disp2,2);
LCDWriteStringXY(15,0,"%");

for(i=0; i<=40; i++)
           {
          
           adc_int[i] = adc_read(0);      // read adc value  
           }

           max=adc_int[0];
 for(i=0; i<=40; i++)
           {   
       
           if(max<adc_int[i])
           max=adc_int[i];
   
           
           }

        adc=max;

        am = (float)(adc*0.005311799738);//  (5*20)/(1.414*13*1024)
        adc = (int)am; 
        //ftoa(am,2,buf);
		
		//unsigned int s=2.9;
        
		//LCDInit(0);
		//LCDClear();
        LCDGotoXY(0,1);
		LCDWriteInt(am*1000,4);
        LCDWriteStringXY(5,1,"mA");
        _delay_ms(700);

}

}