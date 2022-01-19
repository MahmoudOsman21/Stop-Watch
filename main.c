/*
 *  Stop Watch
 *
 *  Created on: May 11, 2021
 *  Author: Mahmoud Ibrahim Osman
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char second = 0;
unsigned char minute = 0;
unsigned char hour = 0;
unsigned char temp=0;

ISR(INT0_vect){
    /*Reset all values*/
    second = 0;
    minute = 0;
    hour = 0;
}

ISR(INT1_vect){
    temp=Seconds_first_Count; /*Save the last count value*/
    TCCR1B&=~(1<<CS10)&~(1<<CS12); /*Turn off the clock*/
}

ISR(INT2_vect){
    Seconds_first_Count=temp; /*Restore the last count value*/
    TCCR1B=(1<<CS10)|(1<<CS12); /*Turn on the clock*/
}

ISR(TIMER1_COMPA_vect){
    second ++;
	if (second == 60){
		second = 0;
		minute ++;
	}
	if (minute == 60){
		second = 0;
		minute = 0;
		hour ++;
	}
	if (hour == 12){
		second = 0;
		minute = 0;
		hour = 0 ;
	}
}


void Timer1_CTC_Init(){
        TCCR1A=(1<<COM1A1)|(1<<FOC1A); /*Turn on compare mode for channel A*/
        TCCR1B=(1<<WGM12)|(1<<CS10)|(1<<CS12); /*Configure clock (N=1024) for channel A*/
        TCNT1=0; /*Set initial value to be zero*/
        OCR1A=15625; /*Set compare value to be 15625*/
        TIMSK|=(1<<OCIE1A); /*Enable Timer 1 to trigger interrupt*/
        SREG=(1<<7); /*Enable the I-bit*/
    }

void INT0_Reset_Init(){
        DDRD &=~(1<<PD2); /*Set PD2 to be input pin*/
        PORTD|=(1<<PD2); /*Enable the internal pull-up*/
        MCUCR|=(1<<ISC01); /*Enable interrupts at falling edge*/
        GICR |=(1<<INT0); /*Enable interrupts for INT0*/
        SREG |=(1<<7); /*Enable the I-bit*/
    }

void INT1_Pause_Init(){
        DDRD &=~(1<<PD3); /*Set PD3 to be input pin*/
        MCUCR =(1<<ISC11)|(1<<ISC10); /*Enable interrupts at rising edge*/
        GICR |=(1<<INT1); /*Enable interrupts for INT1*/
        SREG |=(1<<7); /*Enable the I-bit*/
    }

void INT2_Resume_Init(){
        DDRD  &=~(1<<PB2); /*Set PB2 to be input pin*/
        PORTD |=(1<<PB2); /*Enable the internal pull-up*/
        MCUCSR|=(1<<ISC2); /*Enable interrupts at falling edge*/
        GICR  |=(1<<INT2); /*Enable interrupts for INT2*/
        SREG  |=(1<<7); /*Enable the I-bit*/
    }

void main(void)
{
        DDRC=0x0F; //Configure 7 segment output pins
        DDRA=0x3F; //Configure 7 segment input pins

        Timer1_CTC_Init();
        INT0_Reset_Init();
        INT1_Pause_Init();
        INT2_Resume_Init();


    while(1)
    {
        //Display all counts
        PORTA = (1<<5);
	PORTC = second % 10;
	_delay_ms(5);

	PORTA = (1<<4);
	PORTC = second / 10;
	_delay_ms(5);

	PORTA = (1<<3);
	PORTC = minute % 10;
	_delay_ms(5);

	PORTA = (1<<2);
	PORTC = minute / 10;
	_delay_ms(5);
	    
	PORTA = (1<<1);
	PORTC = hour % 10;
	_delay_ms(5);

	PORTA = (1<<0);
	PORTC = hour / 10;
	_delay_ms(5);
    }

}
