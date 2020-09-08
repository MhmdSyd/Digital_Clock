/*
 * final_project.c
 *
 *  Created on: Aug 25, 2019
 *      Author: Mohamed Elsayed
 */
#include  <avr/io.h>
#include  <avr/interrupt.h>
#include  <util/delay.h>

#undef  F_CPU
#define  F_CPU 1000000UL
#define SEC 55
#define MIN 59
#define HOUR 23
#define INLINE

void PORT_init(void);
void Timer1_init(void);
void INT0_init(void);
void INT1_init(void);
INLINE void edit_Time(void);
enum state {
	Mode_OFF, Mode_ON, switch_press = 0, switch_leave, flag_OFF = 0, flag_ON
};

unsigned char sec = SEC, minut = MIN, hour = HOUR, pause = Mode_OFF,
		flag_Minut = flag_OFF, flag_Hour = flag_OFF;

int main(void) {
	PORT_init();
	Timer1_init();
	INT0_init();
	INT1_init();
	// enable global interrupts
	sei();
	while (1) {
		/* Start of Display*/
		PORTB = (PORTB & 0xC0) | (1 << PB5);
		PORTC = (PORTC & 0xF0) | (sec % 10);
		_delay_us(10);
		PORTB = (PORTB & 0xC0) | (1 << PB4);
		PORTC = (PORTC & 0xF0) | (sec / 10);
		_delay_us(10);
		PORTB = (PORTB & 0xC0) | (1 << PB3);
		PORTC = (PORTC & 0xF0) | (minut % 10);
		_delay_us(10);
		PORTB = (PORTB & 0xC0) | (1 << PB2);
		PORTC = (PORTC & 0xF0) | (minut / 10);
		_delay_us(10);
		PORTB = (PORTB & 0xC0) | (1 << PB1);
		PORTC = (PORTC & 0xF0) | (hour % 10);
		_delay_us(10);
		PORTB = (PORTB & 0xC0) | (1 << PB0);
		PORTC = (PORTC & 0xF0) | (hour / 10);
		_delay_us(10);
		/* End of Display */

		/* Only when  pause mode is ON*/
		if ((Mode_ON == pause)) {
			edit_Time();
		}
	}
	return 0;
}

void PORT_init(void) {
	DDRB |= 0x3F;
	PORTB &= 0xC0;
	DDRC |= 0x0F;
	PORTC &= 0xF0;
	DDRD &= 0xF0;
	PORTD |= 0x0F;
}
void Timer1_init(void) {
	/* set up timer with prescaler = 64 and CTC mode */
	TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10);
	/* initialize counter */
	TCNT1 = 0;
	/* initialize compare value */
	OCR1A = 15625;
	/* enable compare interrupt */
	TIMSK |= (1 << OCIE1A);
}
void INT0_init(void) {
	DDRD &= ~(1 << PD2);
	PORTD |= (1 << PD2);
	GICR |= (1 << INT0);
	MCUCR |= (1 << ISC01);
}
void INT1_init(void) {
	DDRD &= ~(1 << PD3);
	PORTD |= (1 << PD3);
	GICR |= (1 << INT1);
	MCUCR |= (1 << ISC11);
}
INLINE void edit_Time(void) {
	if (!(PIND & (1 << PD0)) && (switch_press == flag_Minut)) {
		minut = (minut + 1) % 60;
		flag_Minut = flag_ON;
	} else {
		if ((PIND & (1 << PD0)) && (switch_leave == flag_Minut))
			flag_Minut = flag_OFF;
	}
	if (!(PIND & (1 << PD1)) && (switch_press == flag_Hour)) {
		hour = (hour + 1) % 24;
		flag_Hour = flag_ON;
	} else {
		if ((PIND & (1 << PD1)) && (switch_leave == flag_Hour))
			flag_Hour = flag_OFF;
	}
	return;
}

ISR (TIMER1_COMPA_vect) {
	if (0 == pause) {
		sec++;
		if (60 == sec) {
			minut++;
			sec = 0;
		}
		if (60 == minut) {
			hour++;
			minut = 0;
		}
		if (24 == hour) {
			hour = 0;
		}
	} else {
	}
}
ISR(INT0_vect) {
	sec = SEC;
	minut = MIN;
	hour = HOUR;
}
ISR(INT1_vect) {
	pause ^= (1 << 0);
}
