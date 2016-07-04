/* i2c.h 2 AUG,07 by lse */ 
#ifndef _I2C_H_
#define _I2C_H_

#ifdef __cplusplus
extern "C" {
#endif    
    
void i2c_init(uint16_t khz);
int i2c_read_bytes(uint8_t dev, uint8_t addr, uint8_t *data, int num);
int i2c_read_bits(uint8_t dev, uint8_t addr, uint8_t bitpos, uint8_t len, uint8_t *data);
int i2c_write_bits(uint8_t dev, uint8_t addr, uint8_t bitpos, uint8_t len, uint8_t data);
int i2c_write_bytes(uint8_t dev, uint8_t addr, uint8_t *data, int num);

#ifdef __cplusplus
}	
#endif

#endif


