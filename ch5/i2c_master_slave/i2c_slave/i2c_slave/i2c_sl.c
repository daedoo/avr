/* i2c.h 17 APR,16 by lse */ 
//------------------------------------------------------
//--  File Name   : i2c_sl.c 
//--  Description : AVR i2c slave read write routines
//--  Date        : 2016. 4. 17
//--  By          : sela (sigsaly@naver.com)
//------------------------------------------------------
//--  Rev :
//------------------------------------------------------
//#include <ctype.h>
//#include <stdint.h>
#include <stdio.h>

#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "i2c.h"

#define I2C_BUFFER_LENGTH 16

#define M_START		(0x08)
#define M_RESTART	(0x10)
#define MT_SLA_ACK	(0x18)
#define MT_DATA_ACK (0x28)
#define MR_SLA_ACK  (0x40)
#define MR_DATA_ACK (0x50)
#define MR_DATA_NACK (0x58)
#define M_STOP		(0x08)

#define I2C_TIMEOUT 5000

#define I2C_DEBUG

#define SR_SLA_ACK	(0x60)
#define SR_DATA_ACK (0x80)
#define SR_DATA_NACK (0x88)
#define SR_STOP		(0xA0)

#define ST_SLA_ACK	(0xA8)
#define ST_DATA_ACK (0xB8)
#define ST_DATA_NACK (0xC0)
#define ST_LAST_DATA (0xC8)

static uint8_t *sl_buffer;
static uint8_t sl_sub_addr;
static uint8_t sl_buffer_len = 0;
static uint8_t receive_cnt = 0;

void i2c_slave_init(uint8_t slv_addr, uint8_t *buffer, uint8_t buf_len)
{
	// set slave address
	TWAR = slv_addr<<1;	
	
	// init avr slave memory 
	sl_buffer = buffer;
	sl_buffer_len = buf_len;
	sl_sub_addr = 0;
	
	// clear int flag, enable TWI
	TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN)|(1<<TWIE);
}

ISR(TWI_vect)
{
	switch(TWSR){
		// for Slave Receiver
		case SR_SLA_ACK:  // transmitter picks me up
			receive_cnt = 0;
			TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN)|(1<<TWIE);
#ifdef I2C_DEBUG			
			printf("SR_SLA_ACK\n");
#endif
			break;
		case SR_DATA_ACK: // data received, ack returned
			if(receive_cnt == 0) //first byte regarded as sub addr 
			{
				sl_sub_addr = TWDR;
				receive_cnt++;
				TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN)|(1<<TWIE);
			}
			else if(sl_sub_addr < sl_buffer_len-1) // buffer has room, return ack
			{   
				sl_buffer[sl_sub_addr++] = TWDR;
				receive_cnt++;
				TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN)|(1<<TWIE);							
			}else // no more room, return nack 
			{ 
				sl_buffer[sl_sub_addr] = TWDR;
				receive_cnt++;
				TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE);
			}
#ifdef I2C_DEBUG
			printf("SR_DATA_ACK, addr: %x, cnt: %d\n", sl_sub_addr, receive_cnt);			
#endif
			break;
		case SR_DATA_NACK: // data received, returned nack
			sl_buffer[sl_sub_addr] = TWDR;
			if(sl_sub_addr < sl_buffer_len-1)
				sl_sub_addr++;
			receive_cnt++;
			TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE);
#ifdef I2C_DEBUG
			printf("SR_DATA_NACK, addr: %x, cnt: %d\n", sl_sub_addr, receive_cnt);
#endif
			break;
		case SR_STOP: // stop or repeated start condition received
			TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN)|(1<<TWIE);
#ifdef I2C_DEBUG
			printf("SR_STOP\n");
#endif
			break;
		
			// Slave Transmitter
		case ST_SLA_ACK:   // addressed, returned ack
			// set data to output register
			TWDR = sl_buffer[sl_sub_addr];
#ifdef I2C_DEBUG
			printf("ST_SLA_ACK, addr: %x\n", sl_sub_addr);
#endif
			TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN)|(1<<TWIE);
			break;
		case ST_DATA_ACK: // byte sent, ack returned		
			// copy data to output register
			TWDR = sl_buffer[++sl_sub_addr];
			// if there is more to send, ack, otherwise nack
			if(sl_sub_addr < sl_buffer_len-1)
			{
				TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN)|(1<<TWIE);
			}
			else
			{
				TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWIE);
			}
#ifdef I2C_DEBUG
			printf("ST_DATA_ACK, addr: %x\n", sl_sub_addr);			
#endif
			break;
		case ST_DATA_NACK: // received nack, nothing to do
		case ST_LAST_DATA: // received ack, nothing to do
#ifdef I2C_DEBUG
			printf("ST_DATA_NACK(or LAST), addr: %x\n", sl_sub_addr);
#endif
			// just set ack condition for next cycle
			TWCR = (1<<TWINT)|(1<<TWEA)|(1<<TWEN)|(1<<TWIE);
			break;
		default:
#ifdef I2C_DEBUG
			printf("default: TWSR %x\n", TWSR);
#endif
			break;
	}
}

