#ifndef _SENSOR_H_
#define _SENSOR_H_

#include <STC12C5A60S2.H>
#include <intrins.h>
#include <math.h>

//30K NTC 热敏电阻连接引脚 另一脚接地
sbit RES_PIN = P1 ^ 0;

#define ADC_SPEED 0	//0-3
#define ADC_R_PIN 0

//温度常量
#define CONST_25ADDK     298.15
#define CONST_K          273.15
//NTC B
#define CONST_B          3950.0
//NTC R25
#define CONST_R          30.0
//ADC最大值
#define CONST_ADC_MAX    1023
//ADC偏移调整
#define CONST_ADJ        1.5
//采样数目
#define CONST_SAMPLE_N   4
//最大有效数值
#define CONST_UPPER_LIM  1000

//ADC初始化
void Init_ADC();
//读ADC值
unsigned int Read_ADC();
//读电阻值
float Read_Res();
//读温度值
float Read_Temperature ();

//光电传感器
sbit PHOTO_PIN = P1 ^ 1;
//判断纸张是否存在
unsigned char IsPaperExist();

#endif