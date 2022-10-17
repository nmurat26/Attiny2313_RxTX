/*

  MAX485                               Attiny2313
+----------+               +--------------------+             
|        RO|-------------->|PD0/RXD		     PB0|             
|A       D1|<--------------|PD1/TXD		     PB1|<---(SH/LD)             
|B      ~RE|<--------------|PD2			     PB2|			  +---------------+
|        DE|<--------------|PD3			     PB3|------------>|	     ROLE	  |	  
+----------+               |PD6              PB4|<----(BUTON) |               |               
						   |					|             +---------------+
						   |                    |
						   |			        |             +---------------+  	
						   |			     PB5|    PB1------|SH/LD		  |
						   |				 PB6|------------>|Q9			  |
						   |				 PB7|------------>|CLK			  |
						   |					|			  |				  |
						   +--------------------+             +---------------+

*/

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
//#define F_CPU   2000000
#include <util/delay.h>

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
volatile unsigned char data[] = "RS 485 TEST MESAJI  ";
/************************************************************/
volatile char readInput(void){
  //output LED
char reader=0;
 DDRB= 0x8A;
 
 /*Raise LD pin*/
 PORTB|=(1<<1);
 
 for (int i=0; i<8; i++)
   {
     reader<<=1;   /*Tek Bitlik Dijital Giriş*/
     reader|=((PINB&0x40)?1:0);
     /*Clock Zamanlaması*/
     PORTB&=~(1<<7); 
     for(int i=0;i<8;i++);
     PORTB|=(1<<7);
    }
 /*LD pini Low Yapılıyor*/
 PORTB&=~(1<<1);
 return reader;
}
void USART_Init( unsigned int baud )
{
	CLKPR = 0B10000000; // Bunlar osilatör ile Alakalı Tam Analmadım
  	CLKPR = 0B00000000; // Clock division factor 1 (0000) 
	  /* Baudrate @ 9600bps Set Edildi */
	UBRRH = (unsigned char)(baud >> 8);
	UBRRL = (unsigned char)baud;	
} 
/*****************************************************************************/
void RS485_Transmit_Byte(char Byte)
{
	
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
/*****************************************************************************/
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
/*******************************************************************************/

int main()
{  
 USART_Init(BAUD_RATE_9600_BPS);//USART Ayarları Yüklendi
 
/*char mesaj = ' ';
DDRB |= (1<<PB3); // PB2,PB3,PB4 all output	
	  
while(1)
	{
		
		mesaj  = RS485_Receive_Byte();   // Wait for PC to send data via  RS485 port 
	
		// use a switch to select appropriate action receiving commands from PC 
		switch (mesaj)
			{
				case 'A':	PORTB |= (1<<PB3); // Switch ON LED D4
							break;
				case 'B':	PORTB &= ~(1 << PB3); // Switch ON LED D4
							break;														
					
			
					
				default: 	PORTB |= (1<<PB3); // Switch OFF LED D4
							
							break;
			}//end of switch
	}
	return 0;*/ 
 	
 /*********************************************************************************/	
 while(1)
    {	
        if (PIND & (1 << PD4)) {    // Buton Basılı Değilse
            PORTB &= ~(1 << PB3);   // Roleyi Kapat Led Kapalı
			_delay_ms(100);
			RS485_Transmit_Byte(UDR);
			//RS485_Receive_Byte();
        }
        else {
            PORTB |= (1 << PB3);    // Buton Baslı DEğil ise Roleyi Aç Ledi Yak
			while (!( UCSRA & (1<<UDRE))); 
			     UDR = readInput();

        }           
    }
	
}


	

  

