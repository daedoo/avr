/*
 * rtc.c, DS1307
 *
 * Created: 2007-08-02
 * Author : sela
 */ 

#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart.h"
#include "i2c.h"
#include "rtc.h"

// based on Zeller's formular
// sun = 0, mon = 1, ..., sat = 6
uint8_t get_day_of_week(int year, int month, int day)
{
	if((month==1)||(month==2))
		year--;
	return (year + year/4 + 6*year/100 + year/400 + 26*(month + 1)/10 + day) % 7;
}

uint8_t bcd2hex(uint8_t value)
{
	uint8_t tmp;

	tmp = (value&0xf0)/16 *10;
	tmp += value&0xf;

	return tmp;
}

int date(void)
{
	uint8_t		dat[8];
	char	*day[] =
	{"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

	if(read_rtc(0, dat, 8) < 0)
		return -1;

	printf("20%02x %x.%x %s %x:%02x:%02x\n",dat[6],dat[5],dat[4],day[dat[3]],dat[2]&0x3f,dat[1],dat[0]&0x7f);
	return 0;
}

int set_date(int yy, int mm, int dd, int hr, int min, int sec)
{
	uint8_t	wdat[7];
	uint8_t dayofweek;

	dayofweek = get_day_of_week(yy,mm,dd);
	
	wdat[0] = sec;			/* seconds */
	wdat[1] = min;			/* minutes */
	wdat[2] = hr;			/* hours, always 24-hour mode */
	wdat[3] = dayofweek;    /* day of week */
	wdat[4] = dd;			/* date    */
	wdat[5] = mm;			/* month   */
	wdat[6] = yy;			/* year    */

	return write_rtc(0, wdat, 7);
}

    
int write_rtc(uint8_t addr, uint8_t *data, int len)
{
	if(i2c_write_bytes(DS1307_ADDR, addr, data, len) < 0)
		return -1;
	else
		return 0;
}

int read_rtc(uint8_t addr, uint8_t *data, int len)
{
	if(i2c_read_bytes(DS1307_ADDR, addr, data, len) < 0)
		return -1;
	else
		return 0;	
}

