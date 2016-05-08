/* ds1307.h 2 AUG,07 by sela */ 
#ifndef _RTC_H_
#define _RTC_H_

int write_rtc(uint8_t addr, uint8_t *data, int len);
int read_rtc(uint8_t addr, uint8_t *data, int len);

int date(void);
int set_date(int yy, int mm, int dd, int hr, int min, int sec);
					
#define DS1307_ADDR	0x68

#endif

