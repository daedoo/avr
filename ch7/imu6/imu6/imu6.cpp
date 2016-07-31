/*
 * imu6.cpp
 *
 * Created: 2016-7-3
 * Author : sela
 */ 
#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>
#include <math.h>
#include "uart.h"
#include "i2c.h"
#include "MPU6050_6AXIS_MOTIONAPPS20.h"

extern "C" void cmd_loop(void);

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high

void dmpDataReady() {
	mpuInterrupt = true;
}

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
	
// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };


int main(void)
{		
	uart_init();
	
	i2c_init(400);
	
	EICRA = (1<<ISC01)|(1<<ISC00); // int0, rising-edge interrupt
	EIMSK = (1<<INT0); // int0 enabled
		
	sei();
			
    MPU6050 mpu;

    if(mpu.testConnection()==true){
        printf("device connection ok\n");
    }
    else{
        printf("device error\n");
        return -1;
    }

    // initialize device
    printf("Initializing devices...\n");
    mpu.initialize();
	
    // load and configure the DMP
    printf("Initializing DMP...\n");
    devStatus = mpu.dmpInitialize();	
	
    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip
	
    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
	    // turn on the DMP, now that it's ready
	    printf("Enabling DMP...\n");
	    mpu.setDMPEnabled(true);

	    // enable Arduino interrupt detection
	    printf("Enabling interrupt detection (external interrupt 0)...\n");
	    //attachInterrupt(0, dmpDataReady, RISING);
	    mpuIntStatus = mpu.getIntStatus();

	    // set our DMP Ready flag so the main loop() function knows it's okay to use it
	    printf("DMP ready! Waiting for first interrupt...\n");
	    dmpReady = true;

	    // get expected DMP packet size for later comparison
	    packetSize = mpu.dmpGetFIFOPacketSize();
	} else {
	    // ERROR!
	    // 1 = initial memory load failed
	    // 2 = DMP configuration updates failed
	    // (if it's going to break, usually the code will be 1)
	    printf("DMP Initialization failed (code %x)", devStatus);
    }	

	uint8_t recv[2];
	uint8_t byts = 0;
	uint32_t cmd_ms;

	while(1){
		if(mpuInterrupt || fifoCount >= packetSize)		
		{
			// reset interrupt flag and get INT_STATUS byte
			mpuInterrupt = false;
			mpuIntStatus = mpu.getIntStatus();
		
			// get current FIFO count
			fifoCount = mpu.getFIFOCount();

			// check for overflow (this should never happen unless our code is too inefficient)
			if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
				// reset so we can continue cleanly
				mpu.resetFIFO();
				printf("FIFO overflow!");

				// otherwise, check for DMP data ready interrupt (this should happen frequently)
			} else if (mpuIntStatus & 0x02) {
				// wait for correct available data length, should be a VERY short wait
				while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();

				// read a packet from FIFO
				mpu.getFIFOBytes(fifoBuffer, packetSize);
			
				// track FIFO count here in case there is > 1 packet available
				// (this lets us immediately read more without waiting for an interrupt)
				fifoCount -= packetSize;			
				
				mpu.dmpGetQuaternion(&q, fifoBuffer);
				mpu.dmpGetGravity(&gravity, &q);
				mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);		
				
				printf("ypr\t %lf\t%lf\t%lf\n", ypr[0] * 180.0/M_PI, ypr[1] * 180/M_PI, ypr[2] * 180/M_PI);					
			}
		}
	}
}

ISR (INT0_vect)
{
	dmpDataReady();
}

