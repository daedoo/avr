/*
 * timer.h
 *
 * Created: 2016-01-02 오후 2:13:22
 *  Author: sela
 */ 

#ifndef TIMER_H_
#define TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif
	
void timer_init(void);
uint32_t get_ms(void);

#ifdef __cplusplus
}
#endif

#endif /* TIMER_H_ */