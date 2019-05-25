/*
 * ATMEGA_328_TIMER.c
 *
 * Created: 28.07.2018 23:54:26
 * Author : ANGRY_ELECTRONICAN
 */ 

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#define F_CPU 8000000UL
#include <util/delay.h>


#include "Timer.h"
#include "Usart.h"

#define LED		PD7
#define PIN_OUT PD3
#define ZERO	PD2

#define TX		PD1
#define RX		PD0

Time time1=0;
void INT0_Init();


ISR(INT0_vect){
	static uint8_t state=0;
	static uint8_t i=0;
	static uint8_t data=0;
	Timer0_StartTimer(&time1);
	EIMSK&=~(1<<INT0);
	switch (state){
	case 0:
			if(Buffer_DataAvailable()){
				data=Buffer_Read();
				PORTD&=~(1<<PIN_OUT);// 1
				PORTD&=~(1<<LED);
				state=1;
			}else{
				PORTD|=(1<<PIN_OUT);// 0
				PORTD|=(1<<LED);
			}
			break;

	case 1:
			if((data>>i)&1){
				PORTD&=~(1<<PIN_OUT);// 1
				PORTD&=~(1<<LED);
			}else{
				PORTD|=(1<<PIN_OUT);// 0
				PORTD|=(1<<LED);
			}
			i++;
			if(i>7){
				i=0;
				state=2;
			}
			break;
		
	case 2:
			if(data%2){
				PORTD|=(1<<PIN_OUT);// 0
				PORTD|=(1<<LED);
				}else{
				PORTD&=~(1<<PIN_OUT);// 1
				PORTD&=~(1<<LED);
				}
			state=0;
			break;
	}
}

int main(void)
{
	DDRD|=(1<<TX)|(1<<LED)|(1<<PIN_OUT);
	PORTD|=(1<<RX)|(1<<PIN_OUT);
	USART_Init();
	Timer0_Init();
	INT0_Init();
	sei();
    while (1)
    {
		cli();
		if(Timer0_TimeIsOut(&time1,5)){
			sei();
			if(EIFR&(1<<INTF0)){
				EIFR|=1<<INTF0;
			}
			EIMSK|=(1<<INT0);
		}
		sei();
    }
}

void INT0_Init(){
	EICRA|=(1<<ISC01)|(1<<ISC00);
	EIMSK|=(1<<INT0);
}
