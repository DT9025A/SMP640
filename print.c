#include "print.h"
#include "uart.h"

#ifdef USE_MX1508
unsigned char code MOTOR_CTRL_TAB[] = {0x00, 0x10, 0x11, 0x01};
#else
unsigned char code MOTOR_CTRL_TAB[] = {0x11, 0x01, 0x00, 0x10};
#endif

/****************
	停步进电机
****************/
void Motor_Stop() {
    MOTOR_A = 0;
    MOTOR_B = 0;
    MOTOR_A_ = 0;
    MOTOR_B_ = 0;
}

/********************
	控制步进电机
	参数:步数0-3
********************/
void Motor_Forward (unsigned char cycle) {
    MOTOR_A = MOTOR_CTRL_TAB[cycle] >> 4 & 1;
    MOTOR_B = MOTOR_CTRL_TAB[cycle] & 1;
    MOTOR_A_ = !MOTOR_A;
    MOTOR_B_ = !MOTOR_B;
}

/***************
	检测SMP640是否存在
	存在:返回0
	不存在:高位返回出错位置,低位返回出错数据
****************/
unsigned int SMP_PrintHead_Exists() {
    unsigned char i, j;
	SMP_Buffer_Return_0();
    SMP_Buffer_Return_0();

    for (i = 0; i < 48; i++) {
        j = SPI_SendByte (0);
        if (j != 0) {
            return i << 8 | j;
        }
    }
    return 0;
}

/*************************
	SMP640的打印数据置0
**********************/
void SMP_Buffer_Return_0() {
    unsigned char i = 0;
    for (i = 0; i < 48; i++)
        SPI_SendByte (0x00);
}

/**********************
	打印一行buffer数据
	参数:行数 1-10		 
**********************
void SMP_Print_Buffer_Line (unsigned char line) {
    unsigned char j;
    unsigned int pos;
    pos = (line - 1) * 48;
    LATCH = 1;
    
    
    LATCH = 0;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    LATCH = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    STB1 = 1;
    //__delay();
    STB1 = 0;
    STB2 = 1;
    //__delay();
    STB2 = 0;
    STB3 = 1;
    //__delay();
    STB3 = 0;
    STB4 = 1;
    //__delay();
    STB4 = 0;
    STB5 = 1;
    // __delay();
    STB6 = 0;
    // __delay();
    if (OnPrintingStep == 4)
        OnPrintingStep = 0;
    Motor_Forward (OnPrintingStep++);
    Motor_Forward (OnPrintingStep++);
    Motor_Stop();
}	   */
