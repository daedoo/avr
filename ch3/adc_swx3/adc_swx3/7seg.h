#ifndef _7SEG_H_
#define _7SEG_H_

//#define CC_MODE
#define CA_MODE

void set_7seg(uint16_t value, uint8_t frac, uint8_t hexa);
uint8_t get_seg_data(uint8_t index);
uint8_t get_seg(uint8_t index);

#endif