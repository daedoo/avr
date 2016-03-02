/*
 * _7seg.c
 *
 * Created: 2015-12-26 오후 1:17:31
 *  Author: sela
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include "7seg.h"

uint8_t seg_buf[4];

//              0  1  2  3  4  5  6  7  8  9  A  b  C  d  E  F DP  null
//PB7 -10 (F)   1  0  0  0  1  1  1  1  1  1  1  1  1  0  1  1  0    0     
//PB6 -11 (A)   1  0  1  1  0  1  1  1  1  1  1  0  1  0  1  1  0    0 
//PB5 - 1 (E)   1  0  1  0  0  0  1  0  1  0  1  1  1  1  1  1  0    0 
//PB4 - 2 (D)   1  0  1  1  0  1  1  0  1  0  0  1  1  1  1  0  0    0 
//PB3 - 3 (DP)  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1    0 
//PB2 - 4 (C)   1  1  0  1  1  1  1  1  1  1  1  1  0  1  0  0  0    0 
//PB1 - 5 (G)   0  0  1  1  1  1  1  0  1  1  1  1  0  1  1  1  0    0 
//PB0 - 7 (B)   1  1  1  1  1  0  0  1  1  1  1  0  0  1  0  0  0    0 
//             F5 05 73 57 87 D6 F6 C5 F7 C7 E7 B6 F0 37 F2 E2 08   00 

uint8_t seg_dat[] =
{
	0xF5, 0x05, 0x73, 0x57, 0x87, 0xD6, 0xF6, 0xC5,
	0xF7, 0xC7, 0xE7, 0xB6, 0xF0, 0x37, 0xF2, 0xE2,
	0x00, 0x02
};

#define DP_CODE	0x08

#ifdef CC_MODE
uint8_t seg_sel[] =
{
	(1<<PD3), (1<<PD4), (1<<PD5), (1<<PD7)
};
#else
uint8_t seg_sel[] =
{
	~(1<<PD3)&((1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD7)),
	~(1<<PD4)&((1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD7)),
	~(1<<PD5)&((1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD7)),
	~(1<<PD7)&((1<<PD3)|(1<<PD4)|(1<<PD5)|(1<<PD7)),
};
#endif

void set_7seg(uint16_t value, uint8_t frac, uint8_t hexa)
{
	uint8_t zero = 1;
	uint16_t seg;
	uint16_t r1,r2,x1,x2,x3;
			
	if(hexa){
		seg = value&0xf000;
		seg >>= 12;
		if(seg > 0)
			zero = 0;		
		if((seg==0)&&(zero=1))
			seg_buf[0] = seg_dat[16];
		else
			seg_buf[0] = seg_dat[seg];

		seg = value&0xf00;
		seg >>= 8;
		if(seg > 0)
			zero = 0;
		if((seg==0)&&(zero=1))
			seg_buf[0] = seg_dat[16];
		else		
			seg_buf[1] = seg_dat[seg];
		
		seg = value&0xf0;
		seg >>= 4;
		if((seg==0)&&(zero=1))
			seg_buf[0] = seg_dat[16];
		else
			seg_buf[2] = seg_dat[seg];
		
		seg = value&0xf;
		seg_buf[3] = seg_dat[seg];
	} 
	else {
		x1 = (uint8_t)(value/1000);
		if(x1 > 0) 
			zero = 0;
		if((x1==0)&&(zero=1))
			seg_buf[0] = seg_dat[16];
		else
			seg_buf[0] = seg_dat[x1]|((frac==3)? DP_CODE:0);
		r1 = value - x1*1000;
		x2 = (uint8_t)(r1/100);
		if(x2 > 0)
			zero = 0;
		if((x2==0)&&(zero=1))
			seg_buf[1] = seg_dat[16];
		else
			seg_buf[1] = seg_dat[x2]|((frac==2)? DP_CODE:0);
		r2 = r1 - x2*100;
		x3 = (uint8_t)(r2/10);
		if((x3==0)&&(zero=1))
			seg_buf[1] = seg_dat[16];
		else
			seg_buf[2] = seg_dat[x3]|((frac==1)? DP_CODE:0);
		seg_buf[3] = seg_dat[(uint8_t)(r2 - x3*10)];					
	}
}

uint8_t get_seg_data(uint8_t index)
{
#ifdef CC_MODE			
	return seg_buf[index];
#else
	return ~seg_buf[index];
#endif
}

uint8_t get_seg(uint8_t index)
{
	return seg_sel[index];
}