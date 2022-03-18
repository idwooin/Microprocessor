// 전자 피아노와 mp3 구현 코드
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdbool.h>
#define F_CPU 16000000UL
#include <util/delay.h> // 무조건 F_CPU 뒤에 위치

#define ON 0
#define OFF 1
#define PLAY 0
#define PERFORM 1

#define DO_data 17
volatile int state = OFF;
volatile int tone;
volatile int play = PLAY;

#define DO 0
#define DOSHARP 1
#define RE 2
#define MIFLAT 3
#define MI 4
#define FA 5
#define FASHARP 6
#define SOL 7
#define SOLSHARP 8
#define RA 9
#define SIFLAT 10
#define SI 11
#define DDO 12
#define RRE 13
#define NON 14
#define EOS -1

char f_table[15] = {17,30, 43, 55, 66, 77, 87, 97, 105 ,114, 122, 130, 137, 150, 255};
int song1[] = {SOL, MI, MI, SOL, MI,DO, RE, MI, RE, DO, MI, SOL, DDO,SOL, DDO, SOL, DDO, SOL, MI, SOL,RE, FA, MI, RE, DO, EOS};
int song2[] = {RE,SOL,SOL,RA,SOL,FASHARP,MI,MI,MI,RA,SI,RA,SOL,FASHARP,RE,RE,SI,DDO,SI,RA,SOL,MI,RE,RE,MI,RA,FASHARP,SOL,EOS};
int song3[] = {SI,RE,MI,SOL,SOL,SI,RE,MI,SI,SOL,SI,RRE,SI,RRE,SI,RA,RA,SOL,RE,SOL,FASHARP,SOL,SOL,FASHARP,RA,SOL,EOS};
int song4[] = {FASHARP,FASHARP,MI,MI,MIFLAT,FASHARP,MIFLAT,DOSHARP,DOSHARP,MIFLAT,DOSHARP,SI,SI,DOSHARP,MIFLAT,DOSHARP,DOSHARP,DOSHARP,SI,MIFLAT,DOSHARP,SI,SOLSHARP,SI,DOSHARP,MIFLAT,DOSHARP,DOSHARP,DOSHARP,EOS};
unsigned char digit[10] = {0x3f, 0x06,0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07,0x7f, 0x67};
unsigned char led[9] = {0x00, 0x80, 0x40,0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
unsigned char compare[9] = {0x00, 0x01, 0x02,0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
int piano[8] = {0,2,4,5,7,9,11,12};
int piano2[9] = {EOS,DO,RE,MI,FA,SOL,RA,SI,DDO};
volatile int whatsong = 0;
volatile int pianosong = 0;
//volatile int j=0;
volatile int i=0;

ISR(INT4_vect){
	if (play == PLAY)
	{
		pianosong = 0;
		PORTA = led[pianosong];
		PORTC = 0x00;
		PORTD = 0x00;
		PORTF = 0x80;
		play = PERFORM;
		_delay_ms(10);
	}
	else
	{
		PORTA = 0x00;
		play = PLAY;
		PORTC = digit[whatsong+1];
		PORTF = 0x00;
		_delay_ms(10);
	}
}

ISR(INT5_vect){
	if(play == PLAY){
		whatsong = (whatsong+1)%4;
		PORTC = digit[whatsong+1];
		_delay_ms(10);
	}
	else{
		
		/*
		j = (j+1)%8;
		PORTA= led[j];
		_delay_ms(10);
		*/
		
	}
}

ISR(TIMER0_OVF_vect)
{
	if (state == ON)
	{
		PORTB = 0x00;
		state = OFF;
	}
	else
	{
		PORTB = 0x10;
		state = ON;
	}
	
	if(play==PLAY){
		TCNT0 = f_table[tone];
	}
	else{
		//TCNT0 = f_table[piano[j]];
		TCNT0 = f_table[tone];
	}
}

void playsong(){
	
	switch(whatsong){
		case 0:
		do{
			tone=song1[i++];
			_delay_ms(500);
			if(play==PERFORM){
				i=0;
				break;
			}
			if(whatsong!=0){
				i=0;
				break;
			}
		}while(tone!=EOS);
		i=0;
		break;
		
		case 1:
		do{
			tone=song2[i++];
			_delay_ms(500);
			if(play==PERFORM){
				i=0;
				break;
			}
			if(whatsong!=1){
				i=0;
				break;
			}
		}while(tone!=EOS);
		i=0;
		break;
		
		case 2:
		do{
			tone=song3[i++];
			_delay_ms(500);
			if(play==PERFORM){
				i=0;
				break;
			}
			if(whatsong!=2){
				i=0;
				break;
			}
		}while(tone!=EOS);
		i=0;
		break;
		
		case 3:
		do{
			tone=song4[i++];
			_delay_ms(500);
			if(play==PERFORM){
				i=0;
				break;
			}
			if(whatsong!=3){
				i=0;
				break;
			}
		}while(tone!=EOS);
		i=0;
		break;
	}
	
}

void piano_play(){
	int it = 0;
	if((PIND & 0x80)==0) it =8;
	if((PIND & 0x40)==0) it = 7;
	if((PIND & 0x20)==0) it = 5;
	if((PIND & 0x10)==0) it = 6;
	if((PIND & 0x08)==0) it = 3;
	if((PIND & 0x04)==0) it = 1;
	if((PIND & 0x02)==0) it = 2;
	if((PIND & 0x01)==0) it = 4;
	if((PIND & 0xff)==0xff) it = 0;
	PORTA = led[it];
	tone = piano2[it];
}

int main(void)
{
	DDRA = 0xff; // LED 출력
	DDRB = 0x10; // 부저 출력
	DDRD = 0x00;
	DDRE = 0xcf; // 5,4번 pin 입력으로
	DDRF = 0x80;
	PORTF = 0x00;
	
	DDRC = 0xff; // C 포트는 모두 출력
	DDRG = 0xff; // G 포트도 4개는 출력
	PORTC = digit[1]; // ‘0’ 표현
	PORTG = 0x01;
	
	TCCR0 = 0x03; // 8분주, 0.5us
	TIMSK = 0x01; // Overflow
	EICRB = 0x0a; // 하강 에지 1010, 인터럽트
	EIMSK = 0x30; // INT5,INT4 interrupt enable
	TCNT0 = f_table[song1[i]]; // 해당 숫자~256까지가 지난 후(overflow) interrupt 발생
	sei(); // interrupt enable

	while(1){
		if(play== PLAY){
			playsong();
		}
		else{
			piano_play();
			
			/*
			while(1){
				piano_play();
				
				if(play==PLAY){
					i=0;
					break;
				}
				
			}
			*/
			
		}
	}
}
