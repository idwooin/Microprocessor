//오디오 스펙트럼 코드
#include<stdint.h>
#include"ffft.h"
#include<math.h>
#include<util/delay.h>
#include<avr/io.h>
#include<avr/interrupt.h>
#include<stdio.h>
#include<stdbool.h>
#include<avr/pgmspace.h>

#define F_CPU 16000000UL

#define byte unsigned char

#define ADC_CHANNEL 0

byte displayData[8][8];	
	
int16_t capture[FFT_N];
complex_t bfly_buff[FFT_N];
uint16_t spectrum[FFT_N/2];
volatile byte samplePos = 0;

  
void setup()
{
	DDRA = 0xFF;
	DDRC = 0XFF;
	PORTA= 0x00;
	PORTC= 0xff;
	ADMUX = ADC_CHANNEL;
	ADCSRA = (1 << ADEN) |
	         (1 << ADSC) |
			 (1 << ADFR)|
			 (1 << ADIE) |
			 (1 << ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	sei(); 	
}

void loop() {
	while(ADCSRA & (1<<ADIE))
	
	fft_input(capture, bfly_buff);
	samplePos = 0;
	ADCSRA |= (1 << ADIE);
	fft_execute(bfly_buff);
	fft_output(bfly_buff, spectrum);
	int i, j,  avg;
	for(i=0;i<8;i++)
	{
		avg = 0;
		for(j=0;j<8;j++)
		{
			avg += spectrum[j+i*8];
		}
		avg /= 8;
		if(avg/(8-i) > 2)
		{
			if(i==0)
			    avg /= 20;
			for(j=0;j<avg/(8-i);j++)
			{
				displayData[i][j] = 1;
			}
		}
	}
	
	for(i=0;i<8;i++)
	{
		PORTA = 0;
		for(j=0;j<8;j++)
		{
			PORTA |= (displayData[7-i][j] << j);
		}
		PORTC = (0xff)^((0x01 << i) % (0xff));
		_delay_ms(2);
	}
	
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			displayData[i][j] = 0;
		}
	}
}

ISR (ADC_vect) {
	static const int16_t noiseThreshold = 4;
	int16_t sample = ADC;
	
	capture[samplePos] = ((sample > (512-noiseThreshold)) &&
	(sample < (512+ noiseThreshold))) ? 0 : sample - 512;
	
	if(++samplePos >= FFT_N) ADCSRA &= ~(1 << ADIE);
	
}


int main()
{
	setup();
	while(1)
	{
		loop();
	}
}
