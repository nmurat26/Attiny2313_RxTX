#include <Arduino.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
//#define F_CPU   2000000
#include <util/delay.h>
#define PIN_D0  ( 0)
#define PIN_D1  ( 1)
#define PIN_A1  ( 2)
#define PIN_A0  ( 3)
#define PIN_D2  ( 4)
#define PIN_D3  ( 5)
#define PIN_D4  ( 6)
#define PIN_D5  ( 7)
#define PIN_D6  ( 8)
#define PIN_B0  ( 9)
#define PIN_B1  (10)
#define PIN_B2  (11)
#define PIN_B3  (12)
#define PIN_B4  (13)
#define PIN_B5  (14)
#define PIN_B6  (15)
#define PIN_B7  (16)
#define PIN_A2  (17) 
/*

  MAX485                               Attiny2313
+----------+               +--------------------+             
|        RO|-------------->|PD0/RXD		     PB0|             
|A       D1|<--------------|PD1/TXD		     PB1|<---(SH/LD)             
|B      ~RE|<--------------|PD2		         PB2|	          +---------------+
|        DE|<--------------|PD3		         PB3|------------>ROLE	          |	  
+----------+               |PD6              PB4|<----(BUTON) |               |               
			               |			        |             +---------------+
				           |                    |
			               |			        |             +---------------+  	
			               |			     PB5|  (13)PB1----|SH/LD(1)		  |
				           |		     (14)PB2|------------>|Q9(9)		  |
				           |		     (19)PB7|------------>|CLK(2)		  |
				           |			  16 PB4|------------>|CE(15)	      |
	                       +--------------------+             +---------------+        j2//gnd,reset(10),scl(13),miso(12),mosi(11),5v
data_pin = 11; // Connect Pin 11 to SER_OUT (serial data out) - Pin 9 (Q7) of 74HC165
shld_pin = 10;  // Connect Pin 10 to SH/!LD (shift or active low load) - Pin 1 (!PL) of 74HC165
clk_pin = 16;  // Connect Pin 16 to CLK (the clock that times the shifting) - Pin 2 (CP) of 74HC165
ce_pin = 13;
*/
const int dataPin = 11;   /* Q7 */
const int clockPin = 16;  /* CP */
const int latchPin = 10;  /* PL */
const int numBits = 8;   
int deger[8]={};
int Cntr;
// +-----------------------------------------------------------------------+ //
// | Dahili  RC 8.0000MHz Attiny2313 Baudrate Değişkenleri                 | //
// +-----------------------------------------------------------------------+ //
#define BAUD_RATE_4800_BPS  103 // 4800bps
#define BAUD_RATE_9600_BPS  52  // 9600bps
#define BAUD_RATE_14400_BPS  34 // 14.4k bps 
#define BAUD_RATE_19200_BPS  25 // 19.2k bps
#define BAUD_RATE_28800_BPS  16 // 28.8k bps
#define BAUD_RATE_38400_BPS  12 // 38.4k bps
/******************************************************/

/************************************************************/
void USART_Init( unsigned int baud )
{
	CLKPR = 0B10000000; // Bunlar osilatör ile Alakalı Tam Analmadım
  	CLKPR = 0B00000000; // Clock division factor 1 (0000) 
	  /* Baudrate @ 9600bps Set Edildi */
	UBRRH = (unsigned char)(baud >> 8);
	UBRRL = (unsigned char)baud;}
void RS485_Transmit_Byte(char Byte)
{
	volatile unsigned char data[] = "RS 485 TEST MESAJI  ";
	// Put MAX485 in Transmit Mode 	
    DDRD  |= (1<<PD2) | (1<<PD3); // PD2,PD3 Output Yapıldı
	PORTD |= (1<<PD3);            // PD3->  DE = High;	                             
	PORTD |= (1<<PD2);            // PD2-> ~RE = High;		
	UCSRB |= (1<<TXEN);         // TX Aktif Edildi
	UCSRC = (0 << USBS) | (3 << UCSZ0);	 
			while ( !( UCSRA & (1<<UDRE)) ); // TX Boşmu Kontrol Edildi	         
	         int i = 0;			
		     while(data[i] != 0)
		        {
			     while (!( UCSRA & (1<<UDRE))); 
			     UDR = data[i];					 
			     i++;                            
		        }
		     while (!( UCSRA & (1<<UDRE))); // TX Gelen Bilgi Kontrol Edildi       
		     UDR = '\r';			
}
char RS485_Receive_Byte()
{	
	char Rxed_Data = '0';	
	DDRD  |=   (1<<PD2) | (1<<PD3);  // PD4 ve PD3 Output  
	PORTD &=  ~(1<<PD3);           // PD4 -->  DE,Logic Low
	PORTD &=  ~(1<<PD2);           // PD3 --> ~RE,Logic Low    
	UCSRB |= (1<<RXEN);           // RX Aktif Et	
	while ( !( UCSRA & (1<<RXC)) ); // Rx Boşmu
	Rxed_Data   = UDR ;             // Gelen Dataları Oku	
	return Rxed_Data;
}
void setup() {
  USART_Init(BAUD_RATE_9600_BPS);//USART Ayarları Yüklendi
  pinMode(dataPin, INPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(12, OUTPUT);
}

void loop() {	  
  //RS485_Transmit_Byte(UDR);  
    byte value = deger[7] << 7 |deger[6] << 6 | deger[5] << 5 |deger[4] << 4 | deger[3] << 3 | deger[2] << 2 | deger[1] << 1 | deger[0];
     if(value == 1){
		//digitalWrite(12, HIGH);		
         char mesaj = ' ';
		 mesaj  = RS485_Receive_Byte();   // Wait for PC to send data via  RS485 port	
		if ( mesaj == 'Z' ){
		    Cntr = 1;
			digitalWrite(12, HIGH);
			delay(150);
			digitalWrite(12, LOW);
			delay(150);
		}
		else if ( Cntr == 1 && mesaj == '0'){
    	    Cntr = 2;
			digitalWrite(12, HIGH);
			delay(150);
			digitalWrite(12, LOW);
			delay(150);
			
    	}
		else if ( Cntr == 2 && mesaj == '0'){
    	    Cntr = 3;
			digitalWrite(12, HIGH);
			delay(150);
			digitalWrite(12, LOW);
			delay(150);
    	}
		else if ( Cntr == 3 && mesaj == '0'){
    	    Cntr = 4;
			digitalWrite(12, HIGH);
			delay(150);
			digitalWrite(12, LOW);
			delay(150);
			//digitalWrite(12, HIGH);
    	}
		else if ( Cntr == 4 && mesaj == '0'){
    	    Cntr = 5;
			digitalWrite(12, HIGH);
			delay(150);
			digitalWrite(12, LOW);
			delay(150);
			//digitalWrite(12, HIGH);
    	}else if ( Cntr == 5 && mesaj == '0'){
    	    Cntr = 6;
			digitalWrite(12, HIGH);
			delay(150);
			digitalWrite(12, LOW);
			delay(150);
			//digitalWrite(12, HIGH);
    	}
		
		
    	else if ( Cntr == 6){
			digitalWrite(12, HIGH);
    		// use a switch to select appropriate action receiving commands from PC 
    		switch (mesaj)
    			{
    				case '1':	digitalWrite(12, HIGH); // Switch ON LED D4
    							break;
    				case '0':	digitalWrite(12, LOW); // Switch ON LED D4
    							break;			
    				default: 	digitalWrite(12, LOW); // Switch OFF LED D4							
    							break;
    			}//end of switch       
    		}
		else {
		    Cntr=0;
		}
      }	

	  


  digitalWrite(latchPin, LOW);
  digitalWrite(latchPin, HIGH);
   for(int i = 0; i<8; i++){     
   //Serial.print(deger[i]);   
   delay(1);
   }    
  for (int i = 0; i < numBits; i++) {
    int bit = digitalRead(dataPin);     
    if (bit == HIGH) {      
      deger[i] = 1;
    } else {      
      deger[i] = 0;
    }    
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);    
  } 
  
 

  
 
}

 

