//***************************************************************
//Universidad del Valle de Guatemala
//Electrónica Digital 2
//Autor: Héctor Alejandro Martínez Guerra
//Hardware: ATMEGA328P
//LAB6
//***************************************************************
//***************************************************************
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

//UART: 9600 bps, 8N1
#define BAUD 9600UL
#define UBRR_VAL ((F_CPU/(16UL*BAUD))-1)		//= 103 para 9600 / 16MHz

static void uart_init(void){
	UBRR0H = (uint8_t)(UBRR_VAL >> 8);
	UBRR0L = (uint8_t)(UBRR_VAL & 0xFF);
	UCSR0B = (1<<TXEN0);						//habilita TX
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);			//8 datos, 1 stop, sin paridad
}

static void uart_tx(uint8_t c){ 
	while(!(UCSR0A & (1<<UDRE0)));				//espera buffer libre
	UDR0 = c;									//escribe el byte a enviar
}

int main(void){
	//PORTC como entradas con pull-up: PC0..PC5
	DDRC  &= ~0x3F;								//0b00111111 -> entradas
	PORTC |=  0x3F;								//pull-ups internos ON

	uart_init();

	uint8_t prev = 0;							//estado anterior "presionado" (activo en 1)
	for(;;){
		//Leer y convertir a "1 = presionado" (pull-up => presiona = 0)
		uint8_t raw   = PINC & 0x3F;			//PC0..PC5
		uint8_t pressed = (~raw) & 0x3F;		//invertir: 1 si está a GND, es decir sí esta presionado 

		//Debounce simple: confirmar 10 ms
		if(pressed){
			_delay_ms(10); 
			raw     = PINC & 0x3F; 
			pressed = (~raw) & 0x3F; 
		}

		//Detectar flanco de bajada: nuevo_press = actual & ~prev
		uint8_t new_press = pressed & ~prev;

		if(new_press & (1<<0)) uart_tx('U');	//PC0 ?
		if(new_press & (1<<1)) uart_tx('D');	//PC1 ?
		if(new_press & (1<<2)) uart_tx('L');	//PC2 ?
		if(new_press & (1<<3)) uart_tx('R');	//PC3 ?
		if(new_press & (1<<4)) uart_tx('A');	//PC4 A
		if(new_press & (1<<5)) uart_tx('B');	//PC5 B

		prev = pressed;
	}
}
