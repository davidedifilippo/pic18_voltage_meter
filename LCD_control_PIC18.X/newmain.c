#include <xc.h>
#include <stdlib.h> 
#include <plib/adc.h> 
#include <plib/xlcd.h> 
#include <plib/delays.h> 

#pragma config FOSC = HS 	
#pragma config WDT = OFF 	
#pragma config LVP = OFF 	
#pragma config PBADEN = OFF 
#pragma config MCLRE = ON 

#define _XTAL_FREQ 4000000

unsigned char CH0_Low; //Contiene gli 8 bit meno significativi 
unsigned char CH0_High; //Contiene i restanti 2 bit  

 //la routine che segue inizia sul vettore delle 
                                //interruzioni



// // This funcon creates seconds delay. The argument speci?es the delay me in seconds. 
// 
void Delay_Seconds(unsigned char s) 
{ unsigned char i,j;
  for(j = 0; j < s; j++) 
    { for(i = 0; i <  100; i++)
       __delay_ms(10); 
    }
}


//------------------------------------
//genera un ritardo multiplo di 100ms
//------------------------------------

void Delay_100ms(unsigned char x){
    
    unsigned char i;
    unsigned char j;
    
    //genero un ritardo di x * 100 ms
    
    for (i=0;i<x;i++)
       for (j=0;j<10;j++) //genero un ritardo di 100 ms
              __delay_ms(10);


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

void interrupt adc_int_handler(){


CH0_Low = ADRESL;
CH0_High = ADRESH; //Salva il risultato

PORTD = CH0_High;

INTCONbits.GIE = 1; //Ri-abilita le interruzioni (sospese via hardware)  
PIR1bits.ADIF = 0; //Resetta il flag che segnala l'interruzione da ADC

}


int main(void)
{
    unsigned long Vin, mV; 
    char op[10];
    unsigned char D; unsigned char F; unsigned char S; unsigned char T; //Formato D,FST
    

/***Configuro le interruzioni***/

INTCONbits.GIE = 1; //Abilita le interruzioni
INTCONbits.PEIE = 1; //Abilita le interruzioni da periferica
PIE1bits.ADIE =1; //Abilita le interruzioni da ADC



/***Configurazione delle porte***/ 

TRISAbits.RA0 = 1; // RA0 input (analogico)

TRISD = 0x00; // set all port D bits to be output


/****Configurazione ADC***/

//Impostiamo il modulo ADC in modo che il dato a 10 bit acquisito sia  
//giustificato a sinistra

ADCON2bits.ADFM = 0; 

//Rendiamo RA0 Analogico e AN1-------> AN11 Digitali

ADCON1bits.PCFG3 = 1;
ADCON1bits.PCFG2 = 1;
ADCON1bits.PCFG1 = 1;
ADCON1bits.PCFG0 = 0; 

//Selezionano il canale 0 di acquisizione

ADCON0bits.CHS0 = 0;
ADCON0bits.CHS1 = 0;
ADCON0bits.CHS2 = 0; 
ADCON0bits.CHS2 = 0;

//Complessivamente selziono la f_ck = f_osc/16
        
ADCON2bits.ADCS0 = 1;
ADCON2bits.ADCS1 = 0;
ADCON2bits.ADCS2 = 1;



// Abilita il modulo ADC

ADCON0bits.ADON = 1; 

//Avvia la conversione A->D

ADCON0bits.GO = 1; 

Delay_Seconds(1); 
OpenXLCD(FOUR_BIT & LINES_5X7);

while(BusyXLCD());                     //Wait if the LCD is busy 
WriteCmdXLCD(DON);
WriteCmdXLCD(BLINK_OFF); // Blink ON 

// Turn Display ON 
while(BusyXLCD());                     // Wait if the LCD is busy 
WriteCmdXLCD(0x06);                    // Move cursor right 
putrsXLCD("VOLTMETER");                // Display heading Delay_Seconds(2);                      // 2 s delay LCD_Clear();                           // Clear display
Delay_Seconds(2);                      // 2 s delay 
LCD_Clear();    

LCD_Move(1,4);
putrsXLCD("V=");

while(1){

mV=0;
Vin=0;

mV = CH0_Low>>6;
Vin = CH0_High<<2;

mV= mV | Vin;

mV = (mV/1023.0)*5000; //Calcolo i millivolt

/****Genero la prima cifra***/

D = mV/1000;
LCD_Move(1,6); 
itoa(op, D, 10);
putcXLCD(op[0]);

/****Inserisco la virgola***/

LCD_Move(1,7); 
putcXLCD('.');

/****Genero la prima cifra***/

F = (mV - D*1000)/100;
LCD_Move(1,8); 
itoa(op, F, 10);
putcXLCD(op[0]);

/****Genero la seconda cifra***/

S= ((mV - D*1000)-F*100)/10;
LCD_Move(1,9); 
itoa(op, S, 10);
putcXLCD(op[0]);

S= (((mV - D*1000)-F*100)-S*10);
LCD_Move(1,10); 
itoa(op, S, 10);
putcXLCD(op[0]);


//itoa(op, mV, 10);                           // Convert mV into ASCII string
// // Display result on LCD // 
//LCD_Move(1,6);                              
// Move to row = 1,column = 6 
//putsXLCD(op);           
Delay_100ms(1); //Una acquisizione al secondo
ADCON0bits.GO = 1; //Avvia la conversione A->D
 
}


return 0; 
}


