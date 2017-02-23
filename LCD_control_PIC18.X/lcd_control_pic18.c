#include <xc.h> 
#include <stdlib.h> 
#include <plib/adc.h> 
#include <plib/xlcd.h> 
#include <plib/delays.h> 

#pragma config MCLRE = EXTMCLR, WDTEN = OFF, FOSC = HSHP 
#define _XTAL_FREQ 8000000

/*
 *  This function creates seconds delay. 
 *  The argument speci?es the delay time in seconds.
 */ 

void Delay_Seconds(unsigned char s) { 
    unsigned char i,j;
    for(j = 0; j < s; j++) 
    {
        for(i = 0; i <  100; i++)__delay_ms(10); 
    }
}


// This funcon creates 18 cycles delay for the xlcd library 

void DelayFor18TCY( void ) 
{   
    Nop(); Nop(); Nop(); Nop(); 
    Nop(); Nop(); Nop(); Nop(); 
    Nop(); Nop(); Nop(); Nop(); 
    Nop(); Nop(); 
    return; 
}

/* This funcon creates 15 ms delay for the xlcd library */ 

void DelayPORXLCD( void ) 
{   __delay_ms(15); 
    return; 
}
// // This funcon creates 5 ms delay for the xlcd library // 

void DelayXLCD( void ) 
{ 
    __delay_ms(5); 
    return; 
}
/* This function clears the screen */ 

void LCD_Clear() 
{   
    while(BusyXLCD()); 
    WriteCmdXLCD(0x01); 
}

/* This function moves the cursor to posion row,column */ 

void LCD_Move(unsigned char row, unsigned char column) 
{   char ddaddr = 40*(row - 1) + column; 
    while( BusyXLCD() ); 
    SetDDRamAddr( ddaddr ); 
} 

void main() { 
    unsigned long Vin, mV; 
    char op[10];

ANSELB = 0;        

// Con?gure PORTB as digital 

ANSELA = 1;       
// COn?gure RA0 as analog 
TRISB = 0;      
// Con?gure PORTB as outputs 
TRISA = 1;                    
// Con?gure RA0 as input
// // Con?gure the LCD to use 4-bits, in mulple display mode // 

Delay_Seconds(1); 
OpenXLCD(FOUR_BIT & LINES_5X7); 

/*
 * Con?gure the A/D converter 
 */


OpenADC(ADC_FOSC_2 & ADC_RIGHT_JUST & ADC_20_TAD, ADC_CH0 & ADC_INT_OFF, 
        ADC_TRIG_CTMU & ADC_REF_VDD_VDD & ADC_REF_VDD_VSS);

while(BusyXLCD());                     //Wait if the LCD is busy 
WriteCmdXLCD(DON);                     // Turn Display ON 
while(BusyXLCD());                     // Wait if the LCD is busy 
WriteCmdXLCD(0x06);                    // Move cursor right 
putrsXLCD("VOLTMETER");                // Display heading 
Delay_Seconds(2);                      // 2 s delay LCD_Clear();                          
// Clear display
LCD_Clear();     

for(;;)                                 // Endless loop 
{ LCD_Clear();                          // Clear LCD 

                                       
SelChanConvADC(ADC_CH0);               // Select channel 0 and start conversion 
while(BusyADC());                          // Wait for compleon 
Vin = ReadADC();       
// Readconverted data m
V = (Vin * 5000) >> 10;      // 

LCD_Move(1,1);                              // Move to row = 1, column = 1 
putrsXLCD("mV = ");         
mV = (Vin * 5000) >> 10;      // 
itoa(op, mV, 10);                           // Convert mV into ASCII string
// // Display result on LCD // 
LCD_Move(1,6);                              // Move to row = 1,column = 6 
putsXLCD(op);                               // Display the measured voltage 
Delay_Seconds(1);               // Wait 1 s 
} 
} 