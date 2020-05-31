/*
 * 5transmit_328p.c
 *
 * Created: 6/13/2019 4:08:23 PM
 * Author : PratikA
 */ 

#define F_CPU 8000000

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/delay.h>


void configure();
void pwm_init();
void timer_0(int);
void adc_init(void);
void adc_start(char);
void uart_init(void);
void uart_tx(char);
char uart_rx(void);
void uart_string(char*);
int maap2(int no);	//index
int maap3(int no);	//middle
int maap4(int no);	//ring
int maap5(int no);	//little
int mapth(int no);	//thumb


char a[6],b[5],waste[10],wasteth[10];
int x,gett=0;
uint8_t feedback;
int max_configure=0,min_configure=1000,previousth=0;


int main(){
	char uartData;
	//DDRA=0x01;
	uart_init();
	adc_init();
	//uart_string("Home Automation\r\n");	
	sei();
	while (1);
}

void uart_init(void)
{
	UBRR0L=51;		//Baud Rate=9600
	UCSR0B=0x18;		
	UCSR0C=0x86;
}


void uart_tx(char data)
{
	UDR0=data;
	while((UCSR0A & (1<<TXC0))==0);
	UCSR0A|=1<<TXC0;
}

char uart_rx(void)
{
	while((UCSR0A & (1<<RXC0))==0);
	return UDR0;
}

void uart_string(char *p)
{

	while(*p!='\0'){
		uart_tx(*p);
		p++;
	}
}

int mapth(int no)
{
	int temp;
	if(no < 500)	temp = 110;
	else if(no>500 && no<800) temp = 130;
	else if(no>800) temp=140;
	return temp;
}

int maap2(int no){
	int temp;
	//temp = -3.33*(no-700); //originally used
	temp = -2.84*(no-711);
	return temp;
}

int maap3(int no){
	int temp;
	
	//temp = -3.33*(no-700); //originally used
	temp = -3.41*(no-750);	//for middle
	return temp;
}

int maap4(int no){
	int temp;
	
	//temp = -3.33*(no-700); //originally used
	temp = -3*(no-638);	//for ring
	return temp;
}

int maap5(int no){
	int temp;
	
	//temp = -3.33*(no-700); //originally used
	temp = 4.5*(no-802);	//for little
	return temp;
}

void adc_init(void){
	ADMUX |= (1<<REFS0) ;

	ADCSRA |= (1<<ADEN) |  (1<<ADPS0) | (1<<ADPS1) | (1<<ADPS2);

	ADCSRA |= (1<<ADIE);
	ADCSRA |= (1<<ADSC);
}

void adc_start(char channel)
{
	char temp;
	temp= channel;
	temp|=0xc0;
	ADMUX = temp;
	//ADCSRA |= (1<<ADSC);
}

void timer_0(int t)
{

	for(int i=0;i<t;i++)
	{
		TCCR1B|=(1<<CS12)|(1<<CS10);	//prescalar 1024
		OCR1A=6000;
		TCNT1=0;
		while((TIFR0 & (1<<OCF1A)) == 0);// wait till the timer overflow flag is SET
		TCNT1 = 0;
		TIFR0 |= (1<<OCF1A) ; //clear timer1 overflow flag
	}
}


ISR(ADC_vect)
{
	int dat,mapvalue,mapvalueth;
	dat=ADC;
	//mapvalue=maap2(dat);

	ADCSRA |= (1<<ADIF);
	//itoa(dat,a,10);
	//itoa(mapvalue,waste,10);


	if(feedback!=0 && feedback!=6)
	{
		//uart_tx(feedback+48);
		if(feedback==1)
		{
			mapvalue=maap2(dat);
			itoa(mapvalue,waste,10);
			uart_string("*a");
			uart_string(waste);
			uart_string("|");
			//uart_string("M:");
			//uart_string(waste);	uart_string("\r\n");
			//	_delay_ms(100);

		}

		else if(feedback==2)
		{
			mapvalue=maap3(dat);
			itoa(mapvalue,waste,10);
			uart_string("*b");
			uart_string(waste);
			uart_string("|");
			//uart_string("M:");
			//uart_string(waste);	uart_string("\r\n");
			//	_delay_ms(100);
		}

		else if(feedback==3)
		{
			mapvalue=maap4(dat);
			itoa(mapvalue,waste,10);
			uart_string("*c");
			uart_string(waste);
			uart_string("|");
			//uart_string("M:");
			//uart_string(waste);	uart_string("\r\n");
			//	_delay_ms(100);
		}

		else if(feedback==4)
		{
			mapvalue=maap5(dat);
			itoa(mapvalue,waste,10);
			uart_string("*d");
			uart_string(waste);
			uart_string("|");
			//uart_string("M:");
			//uart_string(waste);	uart_string("\r\n");
			//	_delay_ms(100);
		}

		else if(feedback==5)
		{
			mapvalueth=mapth(dat);
			if(previousth != mapvalueth){
				itoa(mapvalueth,wasteth,10);
				uart_string("*e");
				uart_string(wasteth);
				uart_string("|");
			}
			previousth = mapvalueth;
		}
	}

	feedback++;
	_delay_ms(10);
	adc_start(feedback);

	if(feedback>5)
	{
		feedback=0;
	}


	/*
	uart_string("*");
	uart_string(a);
	uart_string("|");
	//timer_0(1);
	
	uart_string("\r\n");
	pwm_init();
	x = ADC;
	gett = maap(x,0,255,1,1023);
	itoa(gett,b,10);
	//	uart_string("pwm value");
	//	uart_string(b);
	OCR0 = gett;
	uart_string("\r\n");
	//	timer_0(1);

	ADCSRA |= (1<<ADSC);
	*/
	ADCSRA |= (1<<ADSC);
}
