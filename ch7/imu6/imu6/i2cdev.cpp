// I2Cdev cpp wrapper for AVR Two Wire(i2c)
// only take class structure from Jeff Rowberg' original code
// to interface MPU6050 DMP program
// 2016.4.10 by sela <sigsaly@naver.com>

#include "I2Cdev.h"
#include "i2c.h"

I2Cdev::I2Cdev() {
}

//int8_t I2Cdev::readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, uint16_t timeout) {
int8_t I2Cdev::readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data) {
	uint8_t byt;
	if(i2c_read_bytes(devAddr, regAddr, &byt, 1)==0){
		*data = byt & (1 << bitNum);
		return 1;
	}
	else
		return 0;
}


//int8_t I2Cdev::readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, uint16_t timeout) {
int8_t I2Cdev::readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data) {
	
    uint8_t byt;
    
    //printf("i2c_read_bits\n");
    if(i2c_read_bytes(devAddr, regAddr, &byt, 1)==0){
	    //printf("1:%x\n", byt);
	    uint8_t msk = ((1 << length) - 1) << (bitStart - length + 1);
	    byt &= msk;
	    //printf("2:%x\n", byt);
	    byt >>= (bitStart - length + 1);
	    //printf("3:%x\n", byt);
	    *data = byt;
	    return 1;
    }
    else
		return 0;
}

//int8_t I2Cdev::readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint16_t timeout) {
int8_t I2Cdev::readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data) {
	
	if(i2c_read_bytes(devAddr, regAddr, data, 1) < 0)
		return 0;
	else
		return 1;
}


//int8_t I2Cdev::readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout) {
int8_t I2Cdev::readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data) {
	
	if(i2c_read_bytes(devAddr, regAddr, data, length) < 0)
		return 0;
	else
		return length;
}

bool I2Cdev::writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data) {
	uint8_t b;
	
    if (i2c_read_bytes(devAddr, regAddr, &b, 1) == 0) {
		b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));		
		if(i2c_write_bytes(devAddr, regAddr, &b, 1)==0)
			return true;		
	}
	return false;
}

bool I2Cdev::writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data) {
    uint8_t b;
	
    if (i2c_read_bytes(devAddr, regAddr, &b, 1) == 0) {
        uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
        data <<= (bitStart - length + 1); // shift data into correct position
        data &= mask; // zero all non-important bits in data
        b &= ~(mask); // zero all important bits in existing byte
        b |= data; // combine data with existing byte
		
		if(i2c_write_bytes(devAddr, regAddr, &b, 1)==0)
			return true;
    }
    return false;
}

bool I2Cdev::writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data) {
	
	if(i2c_write_bytes(devAddr, regAddr, &data, 1) < 0)
		return true;
	else
		return false;
}


bool I2Cdev::writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t* data) {
	
	if(i2c_write_bytes(devAddr, regAddr, data, length) < 0)
		return true;
	else
		return false;	
}

bool I2Cdev::writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data) {
	uint8_t byts[2];
	byts[0] = (uint8_t)((data/0x100)>>8);
	byts[1] = (uint8_t)(data&0xff);
	
	if(i2c_write_bytes(devAddr, regAddr, byts, 2) < 0)
		return true;
	else
		return false;
}
