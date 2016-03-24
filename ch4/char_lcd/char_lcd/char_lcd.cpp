/*
 * char_lcd.cpp
 *
 * Created: 2016-03-06 오후 3:28:04
 * Author : sela
 */ 

#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "LiquidCrystal.h"
#include "timer.h"
#include "uart.h"

extern void cmd_loop(void);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int main(void)
{
	timer_init();
	uart_init();

	sei();
	
	lcd.begin(16,2);
	lcd.printf("hello, world!");
	
	cmd_loop();
}

void hello(void)
{
	lcd.setCursor(0,0);
	lcd.printf("hello, world!");

	while(!uart_kbhit()){
		lcd.setCursor(0,1);	
		lcd.printf("%d", get_ms()/1000);	
	}
		
	lcd.clear();
}

void blink(void)
{
	lcd.setCursor(0,0);
	lcd.printf("hello, world!");

	while(!uart_kbhit()){
		lcd.noBlink();
		_delay_ms(3000);
		lcd.blink();
		_delay_ms(3000);
	}
	lcd.noBlink();
	lcd.clear();
}

void cursor(void)
{
	lcd.setCursor(0,0);
	lcd.printf("hello, world!");

	while(!uart_kbhit()){
		lcd.noCursor();
		_delay_ms(500);
		lcd.cursor();
		_delay_ms(500);
	}
	lcd.noCursor();
	lcd.clear();
}

void display(void)
{
	lcd.setCursor(0,0);
	lcd.printf("hello, world!");

	while(!uart_kbhit()){
		lcd.noDisplay();
		_delay_ms(500);
		lcd.display();
		_delay_ms(500);
	}
	lcd.display();
	lcd.clear();
}

void direction(void)
{
	int thisChar = 'a';
	
	lcd.cursor();

	while(!uart_kbhit()){
		if(thisChar == 'm')
			lcd.rightToLeft();
		if(thisChar == 's')
			lcd.leftToRight();
		if(thisChar > 'z'){
			lcd.home();
			thisChar = 'a';
		}
		
		lcd.write(thisChar);
		_delay_ms(1000);
		thisChar++;
	}
	lcd.noCursor();
	lcd.clear();
}

void scroll(void)
{
	lcd.setCursor(0,0);
	lcd.printf("hello, world!");
	_delay_ms(1000);

	while(!uart_kbhit()){
		for(int positionCounter = 0; positionCounter < 13; positionCounter++){
			lcd.scrollDisplayLeft();
			_delay_ms(150);		
		}
		
		for(int positionCounter = 0; positionCounter < 29; positionCounter++){
			lcd.scrollDisplayRight();
			_delay_ms(150);
		}
		
		for(int positionCounter = 0; positionCounter < 16; positionCounter++){
			lcd.scrollDisplayLeft();
			_delay_ms(150);
		}
		_delay_ms(1000);
	}
	lcd.clear();
}

void serial(void)
{
	char buf[64];
	
		
	while(1){
		lcd.clear();
		lcd.setCursor(0,0);		
		if (fgets(buf, sizeof buf, stdin) == NULL)
			break;
		lcd.printf("%s", buf);
		if(strcmp(buf,"exit")==0)
			break;				
	}
	lcd.clear();
}

void setCursor(void)
{
	const int numRows = 2;
	const int numCols = 16;
	
	while(!uart_kbhit()){
	
		for(int thisLetter = 'a'; thisLetter <= 'z'; thisLetter++){
			for(int thisRow = 0; thisRow < numRows; thisRow++){
				for(int thisCol = 0; thisCol < numCols; thisCol++){
					lcd.setCursor(thisCol, thisRow);
					lcd.write(thisLetter);
					_delay_ms(200);
				}
			}
		}
	}
	lcd.home();
	lcd.clear();
}

void autoScroll(void)
{
	while(!uart_kbhit()){
		lcd.setCursor(0,0);
		for(int thisChar = 0; thisChar < 10; thisChar++){
			lcd.printf("%d", thisChar);
			_delay_ms(500);
		}	
		
		lcd.setCursor(16,1);
		lcd.autoscroll();
		
		for(int thisChar = 0; thisChar < 10; thisChar++){
			lcd.printf("%d", thisChar);
			_delay_ms(500);
		}
		
		lcd.noAutoscroll();		
	}
	lcd.noAutoscroll();
	lcd.clear();	
}
	
	
	
	