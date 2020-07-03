/****************************
		SMP640 Driver
		  DT9025A

	STC12C5A60S2@11.0592MHz
		UART@115200bps

PIN MAP:
	P0[7:0] LCD D0-7                   LCD ������

	P1.0    TM	                       SMP640�¶ȼ����������
	P1.1    COLLECTOR OF PHOTO SENSOR  SMP640��ӡֽ���Թ�
	P1.3    STROBE1                    SMP640����ͷ1
	P1.4    LATCH					   SMP640��������
			SPI CONFIG :CPOL=0 CPHA=0
	P1.5	MOSI SI					   SMP640��������
	P1.6	MISO SO					   SMP640�������
	P1.7 	SCLK CLK				   SMP640ʱ����


	P2[4:0] STROBE2-6                  SMP640����ͷ2-6
	P2.6    LCD_RS					   LCD ����ָ��ѡ��
	P2.5    LCD_RW					   LCD ��д
	P2.7    LCD_E					   LCD ʹ��


	P3.0    TX						   TXD
	P3.1    RX						   RXD
	P3.2    A						   SMP640��ֽ���A��   IN1	A-
	P3.3    A-						   SMP640��ֽ���A-��  IN1	A
	P3.4    B						   SMP640��ֽ���B��   IN2	B-
	P3.5    B-						   SMP640��ֽ���B-��  IN2	B
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

#define VERSION_STRING "V1.30"//�汾��
#define BUILD          1250	  //������
#define TIME_STEP      30	  //30*100us=3000us=3ms  

//��������
extern bit isRecvingData;			  //�Ƿ��ڽ�������
//extern bit isChecksumError;			  //�Ƿ�Ϊ�����У���

bit isOnPrinting = 0;                 //�Ƿ��ڴ�ӡ
bit updateStatus = 0;				  //������ʾ��־λ
//bit MotorStop = 1;                    //�Ƿ���Ҫֹͣ���

unsigned int sysTick = 0;			  //ϵͳtick��0-59999, 6s
unsigned int lastPrintIconTick = 0;   //��ӡ��־��˸
unsigned int targetHeatTick = 0xffff; //Ŀ�����tick
unsigned int targetMotorTick = 0xffff;//Ŀ����tick
unsigned int PRPTR = 0;			 	  //��ǰ��ӡ��RAM��
//unsigned int PRAMAvaliable = 0;		  //�������Ƿ���� xxxx xx[10][9] [8][7][6][5] [4][3][2][1]

char xdata UTRAM[50];			      //UART��ӡ������
unsigned char xdata PRAM[480];		  //��ӡ����PRAM/10��
unsigned char OnPrintingStep;  		  //��ǰ��ӡ����
unsigned char PDARKNESS;       	  //��ӡ�Ҷ�

float temperature;           		  //��ӡͷ�������¶�

void Timer0_Init (void);
void IO_Init();
void RefreshStatus();
bit IsLineXDataAvaliable (unsigned char line);
bit TickApproach (unsigned int tick);
unsigned int TickOffset (unsigned int tick);

/*********************
	������
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

    EA = 1;             // �����ж�
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

    //���SMP640�Ƿ����
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

    //���SMP640�¶�
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

    //���ֽ���Ƿ����
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

    //��GUI
    LCM2402_GRAM_Init();
    LCM2402_Print (0x00, " SMP640 Printer ");
    RefreshStatus();

    while (1) {
        temperature = Read_Temperature();
        RefreshStatus();
        if (temperature > 95.0) {
            //��ӡͷ�¶ȹ���
            sprintf (UTRAM, "WARN:Printer Head Overheating! (%4.3fC)\n", temperature);
            UART_SendString (UTRAM);
            updateStatus = 1;
            //�ô�ӡ��־λ0
        } else if (IsPaperExist() == 0) {
            sprintf (UTRAM, "WARN:No Parer\n", temperature);
            updateStatus = 1;
        } else if (isOnPrinting && isRecvingData == 0) {
            //ֱ��ȫ����ӡ
            for (y = 0; y < 10; y++) {
                //��������
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
	���㾭��offset��tick
******************************/
bit TickApproach (unsigned int tick) {
    int x = sysTick - tick;
    if (x <= 5 && x >= -5)
        return 1;
    return 0;
}

/*****************************
	���㾭��offset��tick
******************************/
unsigned int TickOffset (unsigned int offset) {
    offset += sysTick;
    if (offset > 59999)
        offset -= 59999;
    return offset;
}

/*****************************
	T0��ʼ������
	11.0592@100us
******************************/
void Timer0_Init (void) {	//1΢��@11.0592MHz
    AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
    TMOD &= 0xF0;		//���ö�ʱ��ģʽ
    TMOD |= 0x01;		//���ö�ʱ��ģʽ
    TL0 = 0xAE;		//���ö�ʱ��ֵ
    TH0 = 0xFB;		//���ö�ʱ��ֵ
    TF0 = 0;		//���TF0��־
    TR0 = 1;		//��ʱ��0��ʼ��ʱ
    ET0 = 1;
}

/************************
	IO��ʼ������
************************/
void IO_Init() {
    //P0Ϊ��׼˫���
    P0M1 = 0;
    P0M0 = 0;
    //P1[2:0][7:6][4]Ϊ��׼˫��, [3]Ϊ����, [5]Ϊ����̬
    P1M1 = 0x00;
    P1M0 = 0x08;
    //P2[7:5]Ϊ��׼˫��, [4:0]Ϊ����
    P2M1 = 0;
    P2M0 = 0x1F;
    //P3[1:0]Ϊ��׼˫��, [7:2]Ϊ����
    P3M1 = 0;
    P3M0 = 0XFC;
}

/****************
	ˢ��״̬��ʾ
	��ӡ״̬ ֽ����� �¶���� [4]C D�Ҷ�
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
	��ʱ��T0�жϷ�����
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
	����PRAM��ĳ�������Ƿ���Ч
*******************************/
//bit IsLineXDataAvaliable (unsigned char line) {
//    return (PRAMAvaliable & 1 << (line - 1) ? 1 : 0);
//}