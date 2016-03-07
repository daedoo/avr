/*
 * posServo.c
 *
 * Created: 2016-02-14 오후 4:58:17
 * Author : sela
 */ 

#include <stdio.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU     18432000L
#include <util/delay.h>
#include "uart.h"
#include "adc.h"

extern void cmd_loop(void);
extern uint32_t get_ms(void);

float kp = 1.0;
float ki = 0.1;
float kd = 0.3;

int main(void)
{
	DDRB = (1<<PB3);
	DDRD = (1<<PD3);
	
	PORTC = (1<<PC6);
	
	// set timer0, CTC mode
	TCCR0A = (1<<WGM01)|(1<<COM0A0);
	// clk source, freq = 18.432MHz/8 = 230.4kHz
	TCCR0B = (1<<CS01);
	// output compare value for 100us
	OCR0A = 230;  // 10.017kHz
	
	// set timer2 for pwm
	// Fast PWM mode, Clear OC2A,B on Compare Match, set OC2A,B at 0xff
	TCCR2A = (1<<COM2A1)|(1<<COM2B1)|(1<<WGM21)|(1<<WGM20);
	// clk source, pwm freq = 18.432MHz/(8*256) = 9kHz
	TCCR2B = (1<<CS21);

	OCR2A = 0;
	OCR2B = 0;
	
	TIMSK0 |= (1<<OCIE0A);
	
	//EICRA = (1<<ISC01); // int0, falling-edge interrupt
	//EIMSK = (1<<INT0); // int0 enabled
		
	adc_init();
	uart_init();
	
	sei();
	
	printf("ATmega328P monitor\n");
	cmd_loop();
}

void set_pwm(int pwm)
{
	//printf("pwm: %d\n", pwm);
	if(pwm>=0){
		OCR2A = (uint8_t)pwm;
		OCR2B = 0;
	}
	else
	{
		OCR2A = 0;
		OCR2B = (uint8_t)(-1*pwm);
	}
	//printf("OCR2A:%d, OCR2B:%d \n", OCR2A, OCR2B);
}

void kp_ang(uint8_t angle) // input angle 60-240
{
	int target, current;
	int error;
	float kp_err;
	float pwm = 0.0;
	int cnt = 0;
	
	// y = 1023*x/300
	target = (int)(1023.0*(float)angle/260.0);	
	
	while(!uart_kbhit()){

		current = adc_read(0);
		error = target - current;
		kp_err = kp*error;

		pwm = kp_err;
		if(pwm > 255.0) pwm = 255.0;
		else if (pwm <-256.0) pwm = -255.0;
		
		// limit angle 0-180
		if(((current < 190)&&(pwm < 0))||((current > 940)&&(pwm > 0)))
			set_pwm(0);
		else
			set_pwm((int)pwm);
		
		if((cnt%50)==0)
			printf("t:%d, c:%d, err:%d, kp_err:%d, pwm:%d\n", target, current, error, (int)kp_err, (int)pwm);
		cnt++;
		_delay_ms(10);
	}
	set_pwm(0);
}

void ki_ang(uint8_t angle) // input angle 60-240
{
	int target, current;
	int error, old_error = 0;
	float time_diff;
	float ki_err, accum_err = 0.0;
	float pwm;
	uint32_t last_time;
	uint32_t curr_time;
	int cnt = 0;

	target = (int)(1023.0*(float)angle/260.0);
	last_time = get_ms();
	
	while(!uart_kbhit()){
		current = adc_read(0);
		curr_time = get_ms();			
		time_diff = (float)(curr_time - last_time);
		error = target - current;
		accum_err += (float)error * time_diff * 0.001 ;
		ki_err = ki*accum_err;
		pwm = ki_err;
		
		if(pwm > 255.0) pwm = 255.0;
		else if (pwm <-256.0) pwm = -255.0;
		
		if(old_error != 0)
		{
			// stop on error direction change for safety 
			if(((error >= 0)&&(old_error < 0))||((error <= 0)&&(old_error > 0)))
				break;
			else
				set_pwm((int)pwm);
		}		
			
		old_error = error;
		
		if((cnt%50)==0)
			printf("tar:%d, cur:%d, err:%d, acc_e:%lf, ki_e:%lf, pwm:%lf, td:%lf\n", 
				target, current, error, accum_err, ki_err, pwm, time_diff);
		cnt++;
		last_time = curr_time;
		_delay_ms(10);
	}
	set_pwm(0);
}

void kd_ang(void)
{
	int target, current;
	int error;
	float d_err, kd_err, last_err = 0;
	float time_diff;
	float pwm;
	int cnt = 0;
	uint32_t last_time=0;
	uint32_t curr_time;

	last_time = get_ms();
	
	while(!uart_kbhit()){
		target = adc_read(1);			
		// change 0-1023 to 190-940
		target = (int)(0.73*(float)target);
		current = adc_read(0);
		curr_time = get_ms();
		time_diff = (float)(curr_time - last_time);
		
		error = target - current;
		d_err = (error - last_err)/(time_diff*0.001) ;
		kd_err = kd*d_err;

		pwm = kd_err;
		if(pwm > 255.0) pwm = 255.0;
		else if (pwm <-256.0) pwm = -255.0;
		
		if(((current < 190)&&(pwm < 0))||((current > 940)&&(pwm > 0)))
			set_pwm(0);
		else
			set_pwm((int)pwm);
		
		if((cnt%50)==0)
			printf("tar:%d, cur:%d, err:%d, d_e:%lf, kd_e:%lf, pwm:%lf, td:%lf\n",
			target, current, error, d_err, kd_err, pwm, time_diff);
		cnt++;
		last_time = curr_time;
		last_err = error;
		_delay_ms(10);
	}
	set_pwm(0);
}

void kpi_ang(uint8_t angle) // input angle 60-240
{
	int target, current;
	int error;
	float time_diff;
	float kp_err, ki_err;
	float pwm;
	float accum_err = 0.0;
	int cnt = 0;
	uint32_t last_time=0;
	uint32_t curr_time;

	// y = 1023*x/300
	target = (int)(1023.0*(float)angle/260.0);
	last_time = get_ms();
	
	while(!uart_kbhit()){
		current = (adc_read(0)/4)*4;
		curr_time = get_ms();
		time_diff = (float)(curr_time - last_time);
		
		error = target - current;
		accum_err += (float)error * time_diff * 0.001 ;
		kp_err = kp*error;
		ki_err = ki*accum_err;
		pwm = kp_err + ki_err;
	
		if(pwm > 255.0) pwm = 255.0;
		else if (pwm <-256.0) pwm = -255.0;
		
		if(((current < 190)&&(pwm < 0))||((current > 940)&&(pwm > 0)))
			set_pwm(0);
		else
			set_pwm((int)pwm);
		
		if((cnt%50)==0)
			printf("tar:%d, cur:%d, err:%d, acc_e:%lf, kp_e:%lf, ki_e:%lf, pwm:%lf, td:%lf\n",
				target, current, error, accum_err, kp_err, ki_err, pwm, time_diff);
		cnt++;
		last_time = curr_time;
		_delay_ms(10);
	}
	set_pwm(0);
}

void kpid_ang(uint8_t angle) // input angle 60-240
{
	int target, current;
	int error;
	float time_diff;
	float kp_err, ki_err, kd_err;
	float pwm;
	float accum_err = 0.0;
	float d_err;
	float last_err;
	int cnt = 0;
	uint32_t last_time=0;
	uint32_t curr_time;

	// y = 1023*x/300
	target = (int)(1023.0*(float)angle/260.0);
	last_time = get_ms();
	
	while(!uart_kbhit()){
		current = (adc_read(0)/4)*4;
		curr_time = get_ms();
		time_diff = (float)(curr_time - last_time);
		
		error = target - current;
		accum_err += (float)error * time_diff * 0.001 ;
		d_err = (error - last_err)/(time_diff*0.001) ;

		kp_err = kp*error;
		ki_err = ki*accum_err;
		kd_err = kd*d_err;		

		pwm = kp_err + ki_err + kd_err;
		
		if(pwm > 255.0) pwm = 255.0;
		else if (pwm <-256.0) pwm = -255.0;
		
		if(((current < 190)&&(pwm < 0))||((current > 940)&&(pwm > 0)))
			set_pwm(0);
		else
			set_pwm((int)pwm);
		
		if((cnt%50)==0)
			printf("tar:%d, cur:%d, err:%d, acc_e:%lf, d_e:%lf, kp_e:%lf, ki_e:%lf, kd_e:%lf, pwm:%lf, td:%lf\n",
					target, current, error, accum_err, d_err, kp_err, ki_err, kd_err, pwm, time_diff);
		cnt++;
		last_time = curr_time;
		last_err = error;		
		_delay_ms(10);
	}
	set_pwm(0);
}