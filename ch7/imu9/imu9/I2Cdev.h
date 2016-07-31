// I2Cdev cpp wrapper for AVR Two Wire(i2c)
// only take class structure from Jeff Rowberg' original code 
// to interface MPU6050 DMP program
// 2016.4.10 by sela <sigsaly@naver.com>

#ifndef _I2CDEV_H_
#define _I2CDEV_H_

#include <inttypes.h>

class I2Cdev {
    public:
        I2Cdev();
        
        static int8_t readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data);
        static int8_t readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data);
        static int8_t readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data);
        static int8_t readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);

        static bool writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
        static bool writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
        static bool writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
        static bool writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
        static bool writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);

        static uint16_t readTimeout;
};

#endif /* _I2CDEV_H_ */