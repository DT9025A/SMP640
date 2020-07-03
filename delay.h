#ifndef _DELAY_H_
#define _DELAY_H_

void Delay2ms(void);
void Delay100ms(void);
void Delay500ms(void);

#define Delay1s() Delay500ms();Delay500ms()

#endif