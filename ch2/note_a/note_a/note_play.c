/*
 * note_play.c
 *
 * Created: 2015-11-19
 * Author : sela
 */ 

#include <avr/io.h>
#define F_CPU     4000000L
#include <util/delay.h>

#define OUTBIT	(1<<PD5)

struct notes 
{
	uint8_t note;
	uint8_t duration; 
};

uint16_t freq[] =
{
	392, // G4
	415, // Ab4
	440, // A4
	466, // Bb4
	494, // B4
	523, // C5
	554, // Db5
	587, // D5
	622, // Eb5
	659, // E5
	698, // F5
	740, // Gb5
	784, // G5
	831, // Ab5
	880, // A5
	932, // Bb5
	988, // B5
	1047 // C6
};

struct notes a_music[] = 
//{	{c5,8},{b4f,8},{e5f,12},{d5f,4}, 
//	{c5,4},{e5f,4},{a5f,4}, {b5f,4},{e5f,12},{e5,4},
//	{f5,8},{b4f,6},{c5,1},{d5f,1},{e5f,8},{a4,8},
//	{d5f,8},{c5,2},{b4f,2},{a4f,2},{g4,2},{b4f,8},{a4f,8},
//	{0,0}
{	{5,8},{3,8},{8,12},{6,4},
	{5,4},{8,4},{13,4}, {15,4},{8,12},{9,4},
	{10,8},{3,6},{5,1},{6,1},{8,8},{4,8},
	{6,8},{5,2},{3,2},{1,2},{0,2},{3,8},{1,8},
	{0,0}
};

int main(void)
{
	uint8_t i = 0;
	uint8_t duration;
	
	DDRD = OUTBIT;
	// clear counter on compare match, toggle output OC0B on match
	TCCR0A = (1<<COM0B0)|(1<<WGM01); 
	// prescale clk/64 
	TCCR0B = (1<<CS01)|(1<<CS00);
		
    while (1) 
    {
		duration = a_music[i].duration;
		
		if(!duration)
		{
			OCR0A = 0;
			exit(0);
		}
			
		OCR0A = 62500/freq[a_music[i].note];
		i++;
		while(duration>0)
		{
			duration--;
			_delay_ms(250);
		}
    }
}