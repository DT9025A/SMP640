/****************************
		SMP640 Driver
		  DT9025A

	STC12C5A60S2@11.0592MHz
		UART@115200bps

PIN MAP:
	P0[7:0] LCD D0-7                   LCD 数据线

	P1.0    TM	                       SMP640温度检测热敏电阻
	P1.1    COLLECTOR OF PHOTO SENSOR  SMP640打印纸检测对管
	P1.3    STROBE1                    SMP640加热头1
	P1.4    LATCH					   SMP640数据锁存
			SPI CONFIG :CPOL=0 CPHA=0
	P1.5	MOSI SI					   SMP640数据输入
	P1.6	MISO SO					   SMP640数据输出
	P1.7 	SCLK CLK				   SMP640时钟线


	P2[4:0] STROBE2-6                  SMP640加热头2-6
	P2.6    LCD_RS					   LCD 数据指令选择
	P2.5    LCD_RW					   LCD 读写
	P2.7    LCD_E					   LCD 使能


	P3.0    TX						   TXD
	P3.1    RX						   RXD
	P3.2    A						   SMP640走纸电机A端   IN1	A-
	P3.3    A-						   SMP640走纸电机A-端  IN1	A
	P3.4    B						   SMP640走纸电机B端   IN2	B-
	P3.5    B-						   SMP640走纸电机B-端  IN2	B
****************************/
#ifdef _VSCODE
typedef unsigned char bit;
#define xdata static
#define code const
#endif

#include <STC12C5A60S2.H>
#include <stdio.h>
#include <string.h>
#include "sensor.h"
#include "uart.h"
#include "1602.h"
#include "print.h"
#include "delay.h"
#include "spi.h"

#define VERSION_STRING "V1.30"//版本号
#define BUILD          1250	  //构建号
#define TIME_STEP      30	  //30*100us=3000us=3ms  

//变量定义
extern bit isRecvingData;			  //是否在接收数据
//extern bit isChecksumError;			  //是否为错误的校验和

bit isOnPrinting = 0;                 //是否在打印
bit updateStatus = 0;				  //更新显示标志位
//bit MotorStop = 1;                    //是否需要停止电机

unsigned int sysTick = 0;			  //系统tick从0-59999, 6s
unsigned int lastPrintIconTick = 0;   //打印标志闪烁
unsigned int targetHeatTick = 0xffff; //目标加热tick
unsigned int targetMotorTick = 0xffff;//目标电机tick
unsigned int PRPTR = 0;			 	  //当前打印的RAM行
//unsigned int PRAMAvaliable = 0;		  //行数据是否可用 xxxx xx[10][9] [8][7][6][5] [4][3][2][1]

char xdata UTRAM[50];			      //UART打印缓冲区
unsigned char xdata PRAM[480];		  //打印数据PRAM/10行
unsigned char OnPrintingStep;  		  //当前打印步骤
unsigned char PDARKNESS;       	  //打印灰度

float temperature;           		  //打印头加热区温度

void Timer0_Init (void);
void IO_Init();
void RefreshStatus();
bit IsLineXDataAvaliable (unsigned char line);
bit TickApproach (unsigned int tick);
unsigned int TickOffset (unsigned int tick);

/*********************
	主函数
**********************/
void main() {
    unsigned int x;
    unsigned char y;

    sysTick = 0;
    OnPrintingStep = 0;

    IO_Init();
    LCM2402_Init();
    Timer0_Init();
    UART_Init();
    Init_ADC();
    SPI_Init();
    Motor_Stop();

    EA = 1;             // 打开总中断
    LATCH = 1;
	PDARKNESS = 5;

    LCM2402_Print (0x00, " SMP640 Printer ");
    LCM2402_Print (0x04, " Initalizing... ");
    UART_SendString ("------\n");
    Delay500ms();

    LCM2402_Print (0x00, "Self Checking...");
    sprintf (UTRAM, "SMP640 DRIVER %s(build %d) by DT9025A\n", VERSION_STRING, BUILD);
    UART_SendString (UTRAM);
    Delay2ms();
    UART_SendString ("Self checking.\n");

    //检测SMP640是否存在
    x = SMP_PrintHead_Exists();
    if (x == 0) {
        LCM2402_Print (0x40, " Data Trans OK. ");
        UART_SendString ("SMP640 data transfer OK.\n");
    } else {
        UART_SendString ("SMP640 data transfer ERROR!\n");
        sprintf (UTRAM, "Self check paused. (%02X %02X)\n", x >> 8, x & 0xff);
        UART_SendString (UTRAM);
        LCM2402_Print (0x40, " Data Trans ERR ");
        while (1) {
        }
    }
    Delay1s();

    //检测SMP640温度
    LCM2402_GRAM_Init();
    temperature = Read_Temperature();
    if (temperature < -40.0 || temperature > 150.0) {
        sprintf (GRAM, "NTC ERR: % 4.1f", temperature);
        LCM2402_Print (0x40, GRAM);
        LCM2402_PrintChar (0x4F, CHAR_C);
        UART_SendString ("SMP640 temperature test error!\n");
        sprintf (UTRAM, "Self check paused (%4.3fC)\n", temperature);
        UART_SendString (UTRAM);
        while (1) {
        }
    } else {
        sprintf (UTRAM, "SMP640 temperature test OK. (%4.3fC)\n", temperature);
        UART_SendString (UTRAM);
        sprintf (GRAM, "NTC OK: % 4.3f", temperature);
        LCM2402_Print (0x40, GRAM);
        LCM2402_PrintChar (0x4F, CHAR_C);
    }
    Delay1s();

    //检测纸张是否存在
    if (IsPaperExist()) {
        LCM2402_Print (0x40, "  Paper Exists  ");
        UART_SendString ("Paper Exists.\n");
    } else {
        LCM2402_Print (0x40, "Paper NOT Exists.");
        UART_SendString ("Paper NOT Exists.\n");
        while (1) {
        }
    }
    Delay1s();

    //画GUI
    LCM2402_GRAM_Init();
    LCM2402_Print (0x00, " SMP640 Printer ");
    RefreshStatus();

    while (1) {
        temperature = Read_Temperature();
        RefreshStatus();
        if (temperature > 95.0) {
            //打印头温度过高
            sprintf (UTRAM, "WARN:Printer Head Overheating! (%4.3fC)\n", temperature);
            UART_SendString (UTRAM);
            updateStatus = 1;
            //置打印标志位0
        } else if (IsPaperExist() == 0) {
            sprintf (UTRAM, "WARN:No Parer\n", temperature);
            updateStatus = 1;
        } else if (isOnPrinting && isRecvingData == 0) {
            //直接全部打印
            for (y = 0; y < 10; y++) {
                //传输数据
                for (x = y * 48; x < (y + 1) * 48; x++)
                    SPI_SendByte (PRAM[x]);
                LATCH = 0;
                _nop_();
                _nop_();
                _nop_();
                LATCH = 1;
                STB1 = STB2 = STB3 = STB4 = STB5 = STB6 = 1;
                targetHeatTick = TickOffset (PDARKNESS * 5);
                while (TickApproach (targetHeatTick) == 0);
                STB1 = STB2 = STB3 = STB4 = STB5 = STB6 = 0;
                if (OnPrintingStep == 4)
                    OnPrintingStep = 0;
                targetMotorTick = TickOffset (30);
                Motor_Forward (OnPrintingStep++);
                while (TickApproach (targetMotorTick) == 0);
                targetMotorTick = TickOffset (30);
                Motor_Forward (OnPrintingStep++);
                while (TickApproach (targetMotorTick) == 0);
            }
            isOnPrinting = 0;
        }
        if (updateStatus) {
            updateStatus = 0;

        }
    }

}

/*****************************
	计算经过offset的tick
******************************/
bit TickApproach (unsigned int tick) {
    int x = sysTick - tick;
    if (x <= 5 && x >= -5)
        return 1;
    return 0;
}

/*****************************
	计算经过offset的tick
******************************/
unsigned int TickOffset (unsigned int offset) {
    offset += sysTick;
    if (offset > 59999)
        offset -= 59999;
    return offset;
}

/*****************************
	T0初始化函数
	11.0592@100us
******************************/
void Timer0_Init (void) {	//1微妙@11.0592MHz
    AUXR |= 0x80;		//定时器时钟1T模式
    TMOD &= 0xF0;		//设置定时器模式
    TMOD |= 0x01;		//设置定时器模式
    TL0 = 0xAE;		//设置定时初值
    TH0 = 0xFB;		//设置定时初值
    TF0 = 0;		//清除TF0标志
    TR0 = 1;		//定时器0开始计时
    ET0 = 1;
}

/************************
	IO初始化函数
************************/
void IO_Init() {
    //P0为标准双向口
    P0M1 = 0;
    P0M0 = 0;
    //P1[2:0][7:6][4]为标准双向, [3]为推挽, [5]为高阻态
    P1M1 = 0x00;
    P1M0 = 0x08;
    //P2[7:5]为标准双向, [4:0]为推挽
    P2M1 = 0;
    P2M0 = 0x1F;
    //P3[1:0]为标准双向, [7:2]为推挽
    P3M1 = 0;
    P3M0 = 0XFC;
}

/****************
	刷新状态显示
	打印状态 纸张情况 温度情况 [4]C D灰度
*****************/
void RefreshStatus() {
    if (isRecvingData) {
        LCM2402_Print (0x40, " Receiving Data ");
        /*} else if (isChecksumError) {
            LCM2402_Print (0x40, " Checksum ERROR ");
        */
    } else {
        sprintf (GRAM, " %c %c %c %04.0f%c D",
                 isOnPrinting ? CHAR_PING1 : CHAR_IDLE,
                 IsPaperExist() ? CHAR_PAPOK : CHAR_PAPERR,
                 temperature < 95.0 && temperature > -5.0 ? CHAR_TEMPOK : CHAR_TEMPH,
                 temperature,
                 CHAR_C);
        LCM2402_Print (0x40, GRAM);
		sprintf (GRAM, "%02d",(unsigned int)PDARKNESS);
		LCM2402_Print (0x4E, GRAM);
    }
}


/*****************************
	定时器T0中断服务函数
	11.0592@100us
*******************************/
void Timer0_ISR() interrupt 1 {
    TL0 = 0xAE;
    TH0 = 0xFB;
    TF0 = 0;
    if (sysTick % 5000 == 0) {
        updateStatus = 1;
    }
    sysTick += 1;
    if (sysTick == 60000) {
        sysTick = 0;
        
    }
}

/*****************************
	检验PRAM中某行数据是否有效
*******************************/
//bit IsLineXDataAvaliable (unsigned char line) {
//    return (PRAMAvaliable & 1 << (line - 1) ? 1 : 0);
//}