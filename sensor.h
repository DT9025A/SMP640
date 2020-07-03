#ifndef _SENSOR_H_
#define _SENSOR_H_

#include <STC12C5A60S2.H>
#include <intrins.h>
#include <math.h>

//30K NTC ���������������� ��һ�Žӵ�
sbit RES_PIN = P1 ^ 0;

#define ADC_SPEED 0	//0-3
#define ADC_R_PIN 0

//�¶ȳ���
#define CONST_25ADDK     298.15
#define CONST_K          273.15
//NTC B
#define CONST_B          3950.0
//NTC R25
#define CONST_R          30.0
//ADC���ֵ
#define CONST_ADC_MAX    1023
//ADCƫ�Ƶ���
#define CONST_ADJ        1.5
//������Ŀ
#define CONST_SAMPLE_N   4
//�����Ч��ֵ
#define CONST_UPPER_LIM  1000

//ADC��ʼ��
void Init_ADC();
//��ADCֵ
unsigned int Read_ADC();
//������ֵ
float Read_Res();
//���¶�ֵ
float Read_Temperature ();

//��紫����
sbit PHOTO_PIN = P1 ^ 1;
//�ж�ֽ���Ƿ����
unsigned char IsPaperExist();

#endif