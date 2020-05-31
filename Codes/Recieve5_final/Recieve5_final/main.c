/*
 * Recieve_four.c
 *
 * Created: 6/1/2019 2:24:41 PM
 * Author : PratikA
 */ 


#define F_CPU 8000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char dataBuffer[10];
char data[10];
int indexrecieve,middlerecieve,ringrecieve,littlerecieve,thumbrecieve=130;
int indexFeedBack,middleFeedBack,ringFeedBack,littleFeedBack;
uint8_t loop=0;
int i=0,y=0,error=50,j=0;


void indexMotor(int indexrecieve, int indexFeedBack);
void middleMotor(int middlerecieve, int middleFeedBack);
void ringMotor(int ringrecieve, int ringFeedBack);
void littleMotor(int littlerecieve, int littleFeedBack);
void recieve(char *x);
void adcInit(void);
void adcStart(uint8_t channel);
void uart_init();
char uart_recieve();
void uart_transmit(char x);
void uart_string(char *x);
void recieve(char *data);
void timerInit();
void WDT_ON();
void WDT_off(void);
void servoInit();
void servo(int x);
void servomove(int thumbrecieve);


int main(void)
{
    uart_init();
	servoInit();
	servo(155);
	adcInit();
	timerInit();
	adcStart(0);
	uart_string("Hello World");
	sei();
    while (1);
}


void WDT_ON()
{
	/*
	Watchdog timer enables with typical timeout period 1s 
	second.
	*/
	WDTCR = (1<<WDE)|(1<<WDP2);
}

void WDT_off(void)
{
	/* Write logical one to WDTOE and WDE */
	WDTCR = (1<<WDTOE) | (1<<WDE);
	/* Turn off WDT */
	WDTCR = 0x00;
}


void adcInit(void)
{
	ADMUX=0x00;
	ADCSRA=0x8F;  //ADC Enable, ADIE Enable, Presacller 128
}

void adcStart(uint8_t channel){
	uint8_t temp;
	temp=channel;
	temp|=0xC0;
	ADMUX=temp;
	ADCSRA|=1<<ADSC;
}

void recieve(char *x){
	//uart_string("recieve");
	if(x[0]=='a'){
		uart_string("a");
		memmove(data,data+1,strlen(data));
		uart_string(data);
		indexrecieve=atoi(data);
	}
	else if(x[0]=='b'){
		uart_string("b");
		memmove(data,data+1,strlen(data));
		uart_string(data);
		middlerecieve=atoi(data);
	}
	else if(x[0]=='c'){
		uart_string("c");
		memmove(data,data+1,strlen(data));
		uart_string(data);
		ringrecieve=atoi(data);
	}
	else if(x[0]=='d'){
		uart_string("d");
		memmove(data,data+1,strlen(data));
		uart_string(data);
		littlerecieve=atoi(data);
	}
	
	else if(x[0]=='e'){
		uart_string("e");
		memmove(data,data+1,strlen(data));
		uart_string(data);
		thumbrecieve=atoi(data);
	}
}

void timerInit(void)
{
	TCCR0=0x0D; //CTC mode , 1/1024
	OCR0=20;  //aprox 2.5 ms at 8 MHz
	TIMSK=1<<OCIE0;
}

void uart_init(void){
	UBRRL=51;   //Baud rate 9600 at 8 MHz Crystal
	UCSRB=0x98;  //0X98 //Enable Rx and Tx  and Rx Interrupt enable
	UCSRC=0x86;  //8 bit, 1 stop, bit parity disable
}


void uart_transmit(char x){
	UDR=x;  //usart i/o data register
	while((UCSRA & (1<<TXC))==0);
	UCSRA|=1<<TXC;
}


void uart_string(char *x){
	int z=0;
	while(x[z] != 0){
		uart_transmit(x[z]);
		z++;
	}
}


char uart_recieve(void){
	while((UCSRA & (1<<RXC))==0);
	return UDR;
}

ISR(USART_RXC_vect){
	//WDT_ON();	
	dataBuffer[i]=UDR;
	
	if (dataBuffer[i]=='*')
	{
		//WDT_off();
		//WDT_ON();
		i=0;
		y=1;
	}
	else if(y){
		if (dataBuffer[i]=='|')
		{
			dataBuffer[i]='\0';
			//WDT_off();
			//WDT_ON();
			strcpy(data,dataBuffer);
			//uart_string("InsideUART_ISR");
			recieve(data);
			i=0;
			y=0;
		}
		else{
			i++;
			if (i>8)
			{
				i=0;
				y=0;
				//WDT_off();
				//WDT_ON();
			}
		}
	}
}

ISR(ADC_vect){
	int adcValue;
	char waste[10];
	adcValue=ADC;
	switch(loop){
		case 0:
		indexFeedBack=adcValue;			//indexfeedback is the location of index finger in Bionic Hand
		//WDT_off();
		//WDT_ON();
		//itoa(indexFeedBack,waste,10);	uart_string("I");	uart_string(waste);     uart_string("\n");
		break;
		
		case 1:
		middleFeedBack=adcValue;
		//WDT_off();
		//WDT_ON();
		//itoa(middleFeedBack,waste,10);	uart_string("m");	uart_string(waste);     uart_string("\n");
		break;
		
		case 2:
		ringFeedBack=adcValue;
		//WDT_off();
		//WDT_ON();
		//itoa(ringFeedBack,waste,10);	uart_string("R");	uart_string(waste);     uart_string("\n");
		break;
		
		case 3:
		littleFeedBack=adcValue;
		//WDT_off();
		//WDT_ON();
		//itoa(littleFeedBack,waste,10);	uart_string("L");	uart_string(waste);     uart_string("\n");
		break;
	}
	loop++;
	if(loop>3)	loop=0;
	adcStart(loop);
}

ISR(TIMER0_COMP_vect){
	
	int target1, position1;
	int target2, position2;
	int target3, position3;
	int target4, position4;
	
		target1=indexrecieve;
		position1=indexFeedBack;
		indexMotor(target1,position1);
		
	
		target2=middlerecieve;
		position2=middleFeedBack;
		middleMotor(target2,position2);
		
	
		target3=ringrecieve;
		position3=ringFeedBack;
		ringMotor(target3,position3);
		
	
		target4=littlerecieve;
		position4=littleFeedBack;
		littleMotor(target4,position4);
		
		//if(j==100){
			servomove(thumbrecieve);	
		//	j=-1;
		//}
		//j++;
		
}



void indexMotor(int indexrecieve, int indexFeedBack){
	if(indexrecieve>=indexFeedBack-error && indexrecieve<=indexFeedBack+error){
		//stop indexmotor
		WDT_off();
		WDT_ON();
		PORTB&=~(1<<0);
		PORTB&=~(1<<1);
	}
	
	else if(indexrecieve>indexFeedBack+error){
		//index finger up
		WDT_off();
		WDT_ON();
		PORTB&=~(1<<0);
		PORTB|=(1<<1);
	}
	else if(indexrecieve<indexFeedBack-error){
		//index finger down
		WDT_off();
		WDT_ON();
		PORTB&=~(1<<1);
		PORTB|=(1<<0);
	}
	else{
		//stop indexmotor
		WDT_off();
		WDT_ON();
		PORTB&=~(1<<0);
		PORTB&=~(1<<1);
	}
}

void middleMotor(int middlerecieve, int middleFeedBack){
	
	if(middlerecieve>=middleFeedBack-error && middlerecieve<=middleFeedBack+error){
		WDT_off();
		WDT_ON();
		PORTB&=~(1<<3);
		PORTB&=~(1<<2);
	}
	
	else if(middlerecieve>middleFeedBack+error){
		//index finger up
		WDT_off();
		WDT_ON();
		PORTB&=~(1<<2);
		PORTB|=(1<<3);
	}
	else if(middlerecieve<middleFeedBack-error){
		//index finger down
		WDT_off();
		WDT_ON();
		PORTB&=~(1<<3);
		PORTB|=(1<<2);
	}
	else{
		//stop indexmotor
		WDT_off();
		WDT_ON();
		PORTB&=~(1<<3);
		PORTB&=~(1<<2);
	}
}

void ringMotor(int ringrecieve, int ringFeedBack){
	
	if(ringrecieve>=ringFeedBack-60 && ringrecieve<=ringFeedBack+60){
		//stop indexmotor
		
		WDT_off();
		WDT_ON();
		PORTC&=~(1<<1);
		PORTC&=~(1<<0);
	}
	
	else if(ringrecieve>ringFeedBack+60){
		//index finger up
		WDT_off();
		WDT_ON();
		PORTC&=~(1<<0);
		PORTC|=(1<<1);
	}
	else if(ringrecieve<ringFeedBack-60){
		//index finger down
		WDT_off();
		WDT_ON();
		PORTC&=~(1<<1);
		PORTC|=(1<<0);
	}
	else{
		//stop indexmotor
		WDT_off();
		WDT_ON();
		PORTC&=~(1<<1);
		PORTC&=~(1<<0);
	}
}

void littleMotor(int littlerecieve, int littleFeedBack){
	
	if(littlerecieve>=littleFeedBack-25 && littlerecieve<=littleFeedBack+25){
		//stop indexmotor
		WDT_off();
		WDT_ON();
		PORTC&=~(1<<2);
		PORTC&=~(1<<3);
	}
	
	else if(littlerecieve>littleFeedBack+25){
		//index finger up
		WDT_off();
		WDT_ON();
		PORTC&=~(1<<2);
		PORTC|=(1<<3);
	}
	else if(littlerecieve<littleFeedBack-25){
		//index finger down
		WDT_off();
		WDT_ON();
		PORTC&=~(1<<3);
		PORTC|=(1<<2);
	}
	else{
		//stop indexmotor
		WDT_off();
		WDT_ON();
		PORTC&=~(1<<2);
		PORTC&=~(1<<3);
	}
}

void servoInit()
{
	DDRD|=1<<PD5; // OC1A as output
	TCCR1A=0x82;  //Non inverting FAST PWM mode 14
	TCCR1B=0x1A;  //Prescaler 8   Timer frequency 1 us
	ICR1=19999; //50 Hz PWM Frequency
	//uart_string("Servo Initialised\n");
}

void servo(int x){
	int i;
	
	i=400 + (x*10.3);
	OCR1A=i;
}


void servomove(int thumbrecieve){
	WDT_off();
	WDT_ON();
	servo(thumbrecieve);
	
}

