/* i2c.h 2 AUG,07 by lse */ 
//------------------------------------------------------
//--  File Name   : i2c.c 
//--  Description : AVR i2c read write routines
//--  Date        : 2007. 10. 2
//--  By          : sela (sigsaly@naver.com)
//------------------------------------------------------
//--  Rev :
//------------------------------------------------------
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "i2c.h"

#define M_START		(0x08)
#define M_RESTART	(0x10)
#define MT_SLA_ACK	(0x18)
#define MT_DATA_ACK (0x28)
#define MR_SLA_ACK  (0x40)
#define MR_DATA_ACK (0x50)
#define MR_DATA_NACK (0x58)
#define M_STOP		(0x08)

#define I2C_TIMEOUT 5000

#define START_TIMEOUT		(0xf0)
#define RESTART_TIMEOUT		(0xf1)
#define DEV_ADDR_TIMEOUT	(0xf2)
#define MT_DATA_TIMEOUT     (0xf3)
#define MR_DATA_TIMEOUT		(0xf4)
#define STOP_TIMEOUT		(0xf5)

#define I2C_ERR_START		(0xe0)
#define I2C_ERR_RESTART		(0xe1)
#define I2C_ERR_DEV_ADDR	(0xe2)
#define I2C_ERR_MT_DATA_ACK (0xe3)
#define I2C_ERR_MR_DATA_ACK		(0xe4)
#define I2C_ERR_MR_DATA_NACK	(0xe5)
#define I2C_ERR_STOP		(0xe6)

uint8_t i2c_err = 0;

//#define I2C_DEBUG

int i2c_write_bytes(uint8_t dev, uint8_t addr, uint8_t *data, int len);
int i2c_read_bytes(uint8_t dev, uint8_t addr, uint8_t *data, int len);

void i2c_set_speed(uint16_t khz)
{
	if((khz < 50)||(khz > 400))
		khz = 100;
	TWSR = 0x00; // prescaler(TWPS) = 00, 1
	TWBR = (F_CPU/1000-16*khz)/(2*khz);
}

void i2c_init(uint16_t khz)
{
	// pull-up
	PORTC |= (1<<PC4)|(1<<PC5);

	i2c_set_speed(khz);

	// clear int flag, enable TWI
	TWCR = (1<<TWINT)|(1<<TWEN);
}


int i2c_read_bits(uint8_t dev, uint8_t addr, uint8_t bitpos, uint8_t len, uint8_t *data)
{
    uint8_t byt;
    
    if(i2c_read_bytes(dev, addr, &byt, 1)==0){
        uint8_t msk = ((1<<len)-1) << (bitpos-len+1);
        byt &= msk;
        byt >>= (bitpos-len+1);
        *data = byt;
        return 0;
    }
    else
        return -1;
}

int i2c_write_bits(uint8_t dev, uint8_t addr, uint8_t bitpos, uint8_t len, uint8_t data)
{
    uint8_t byt;
    
    if(i2c_read_bytes(dev, addr, &byt, 1)==0){
        uint8_t msk = ((1<<len)-1) << (bitpos-len+1);
        data <<= (bitpos-len+1);
        data &= msk;
        byt &= ~(msk);
        byt |= data;
        return i2c_write_bytes(dev, addr, &byt, 1);
    }
    else
        return -1;
}

int i2c_wait_int(int timeout)
{
	int cnt = 0;	
	while (!(TWCR & (1<<TWINT)))
	{
		_delay_us(10);
		cnt++;
		if(cnt > timeout)
			return -1;
	};
	return 0;	
}

int i2c_wait_stop(int timeout)
{
	int cnt = 0;
	while (TWCR & (1<<TWSTO))
	{
		_delay_ms(10);
		cnt++;
		if(cnt > timeout)
			return -1;
	};
	return 0;
}

int i2c_set_start(void)
{
#ifdef I2C_DEBUG
	printf("W_START-");
#endif
	TWCR = (1<<TWSTA)|(1<<TWINT)|(1<<TWEN);
	
	if(i2c_wait_int(I2C_TIMEOUT) != 0){
		i2c_err = START_TIMEOUT;
		return -1;
	}
	
	if ((TWSR & 0xF8) != M_START){
		i2c_err = I2C_ERR_START;
		return -1;
	}
	return 0;
}

int i2c_set_restart(void)
{
#ifdef I2C_DEBUG
	printf("W_RESTART-");
#endif
	TWCR = (1<<TWSTA)|(1<<TWINT)|(1<<TWEN);
	
	if(i2c_wait_int(I2C_TIMEOUT) != 0){
		i2c_err = RESTART_TIMEOUT;
		return -1;
	}
	
	if ((TWSR & 0xF8) != M_RESTART){
		i2c_err = I2C_ERR_RESTART;
		return -1;
	}
	return 0;
}

int i2c_set_dev_addr(uint8_t addr, uint8_t rd)
{
	uint8_t status;
	
#ifdef I2C_DEBUG
	printf("DEV_ADDR-");
#endif	
	// send device address
	TWDR = (addr<<1)|rd;
	TWCR = (1<<TWINT) | (1<<TWEN);

	if(i2c_wait_int(I2C_TIMEOUT) !=0){
		i2c_err = DEV_ADDR_TIMEOUT;
		return -1;
	}

	if(rd) 	
		status = MR_SLA_ACK;
	else	
		status = MT_SLA_ACK;

	if ((TWSR & 0xF8) != status){
		i2c_err = I2C_ERR_DEV_ADDR;
		return -1;
	}
	return 0;
}

int i2c_mt_send_data(uint8_t *data, int len)
{
	int i;
	
	// send data
	for(i=0; i<len; i++)
	{
#ifdef I2C_DEBUG
		printf("DATA-");
#endif
		TWDR = *data++;
		TWCR = (1<<TWINT) | (1<<TWEN);

		if(i2c_wait_int(I2C_TIMEOUT) !=0){
			i2c_err = MT_DATA_TIMEOUT;
			return -1;
		}

		if ((TWSR & 0xF8) != MT_DATA_ACK){
			i2c_err = I2C_ERR_MT_DATA_ACK;
			return -1;
		}			
	}	
	return 0;
}

int i2c_mr_receive_data(uint8_t *data, int len)
{
	int i;
	
	for(i=0; i<len-1; i++)
	{
		// send ACK
		TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
		
		if(i2c_wait_int(I2C_TIMEOUT) !=0){
			i2c_err = MR_DATA_TIMEOUT;
			return -1;
		}

		if ((TWSR & 0xF8) != MR_DATA_ACK){
			i2c_err = I2C_ERR_MR_DATA_ACK;		
			return -1;
		}
		*data++ = TWDR;
	}
	// last byte - send NACK
	TWCR = (1<<TWINT)| (1<<TWEN);

	if(i2c_wait_int(I2C_TIMEOUT) !=0){
		i2c_err = MR_DATA_TIMEOUT;
		return -1;
	}
	
	if ((TWSR & 0xF8) != MR_DATA_NACK){
		i2c_err = I2C_ERR_MR_DATA_NACK;
		return -1;
	}
	*data++ = TWDR;	
	
	return 0;
}

int i2c_set_stop(void)
{
	// send STOP
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	
	// wait for TWSTO cleared
	//if(i2c_wait_stop(I2C_TIMEOUT) != 0){
	//	i2c_err = STOP_TIMEOUT;
	//	return -1;
	//}
	return 0;
}


int i2c_write_bytes(uint8_t dev, uint8_t addr, uint8_t *data, int len)
{
	if(i2c_set_start()==0){
		if(i2c_set_dev_addr(dev, 0)==0){
			if(i2c_mt_send_data(&addr, 1)==0){
				if(i2c_mt_send_data(data, len)==0){
					if(i2c_set_stop()==0)
						return 0;
				}
			}
		}
	}
	i2c_set_stop();
	return -1;
}

int i2c_sub_read_bytes(uint8_t dev, uint8_t *data, int len){
	if(i2c_set_restart()==0){
		if(i2c_set_dev_addr(dev, 1)==0){
			if(i2c_mr_receive_data(data, len)==0){
				if(i2c_set_stop()==0)
					return 0;
			}
		}
	}
	i2c_set_stop();
	return -1;
}

int i2c_read_bytes(uint8_t dev, uint8_t addr, uint8_t *data, int len){
	if(i2c_set_start()==0){
		if(i2c_set_dev_addr(dev, 0)==0){
			if(i2c_mt_send_data(&addr, 1)==0){
				if(i2c_sub_read_bytes(dev, data, len)==0){
					if(i2c_set_stop()==0){	
						return 0;
					}
				}
			}
		}
	}
	i2c_set_stop();
	return -1;
}


