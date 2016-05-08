#ifndef _E2P_24C32_H_
#define _E2P_24C32_H_

#ifdef __cplusplus
extern "C" {
#endif    

#define E2P_ADDR 0x50    

#define E2P_PAGE_LEN 16
#define E2P_MAX_ADDR 4096    

int write_e2p16(uint16_t addr, uint8_t *data, int len);
int read_e2p16(uint16_t addr, uint8_t *data, int len);

#ifdef __cplusplus
}	
#endif

#endif


