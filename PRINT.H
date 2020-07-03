#ifndef _PRINT_H_
#define _PRINT_H_

#include <STC12C5A60S2.H>
#include <intrins.h>
#include "1602.h"
#include "spi.h"

sbit LATCH = P1 ^ 4;

sbit STB1 = P1 ^ 3;
sbit STB2 = P2 ^ 4;
sbit STB3 = P2 ^ 3;
sbit STB4 = P2 ^ 2;
sbit STB5 = P2 ^ 1;
sbit STB6 = P2 ^ 0;

//每个驱动波形延迟3ms
sbit MOTOR_A = P3 ^ 2;
sbit MOTOR_A_ = P3 ^ 3;
sbit MOTOR_B = P3 ^ 4;
sbit MOTOR_B_ = P3 ^ 5;

void Motor_Stop();
void Motor_Forward (unsigned char cycle);
void SMP_Print_Buffer_Line (unsigned char a);
unsigned int SMP_PrintHead_Exists();
void SMP_Buffer_Return_0();

#define USE_MX1508
#endif