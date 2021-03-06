#include "1602.h"
char xdata GRAM[20];

/********************************************************************************************
// 读LCM忙程序 [底层协议] // （所有底层协议都无需关注）
// LCM2402测忙，若LCM2402处于忙状态，本函数将等待至非忙状态 //
/********************************************************************************************/
void LCM2402_TestBusy (void) {
    LCM2402_DB0_DB7 = 0xff;		//设备读状态
    LCM2402_RS = 0;
    LCM2402_RW = 1;
    LCM2402_E = 1;
    while (LCM2402_Busy);		//等待LCM不忙
    LCM2402_E = 0;				//
}
/********************************************************************************************
// 写指令程序 //
// 向LCM2402写命令 本函数需要1个指令集的入口参数 //
/********************************************************************************************/
void LCM2402_WriteCMD (unsigned char LCM2402_command) {
    LCM2402_TestBusy();
    LCM2402_DB0_DB7 = LCM2402_command;
    LCM2402_RS = 0;
    LCM2402_RW = 0;
    LCM2402_E = 1;
    LCM2402_E = 0;
}
/********************************************************************************************
// 写数据程序 //
// 向LCM2402写数据 //
/********************************************************************************************/
void LCM2402_WriteData (unsigned char LCM2402_data) {
    LCM2402_TestBusy();
    LCM2402_DB0_DB7 = LCM2402_data;
    LCM2402_RS = 1;
    LCM2402_RW = 0;
    LCM2402_E = 1;
    LCM2402_E = 0;
}
/********************************************************************************************
// 打印字符串程序 // （本函数调用指针函数）
// 向LCM发送一个字符串,长度48字符之内
// 第一行位置 0x00~0x17  第二行位置 0x40~0x57
// 应用举例：print(0x80,"doyoung.net"); //在第一行第一位处从左向右打印doyoung.net字符串
/********************************************************************************************/
void LCM2402_Print (unsigned char a, unsigned char *str) {
    LCM2402_WriteCMD (a | 0x80);
    while (*str != '\0') {
        LCM2402_WriteData (*str++);
    }
    *str = 0;
}
/********************************************************************************************
// 打印单字符程序 //
// 第一行位置 0x00~0x17  第二行位置 0x40~0x57
// 向LCM发送一个字符,以十六进制（0x00）表示
// 应用举例：print(0xc0,0x30); //在第二行第一位处打印字符“0”
/********************************************************************************************/
void LCM2402_PrintChar (unsigned char a, unsigned char t) {
    LCM2402_WriteCMD (a | 0x80);
    LCM2402_WriteData (t);
}
/********************************************************************************************
// 定义小汉字 //
// 可写入8个自字义字符，写入后可用其CGRAM代码直接提取显示。
// 字符定义方法请参考技术手册
/********************************************************************************************/
unsigned char code Xword[] = {
    0X00, 0X15, 0X0E, 0X15, 0X1F, 0X11, 0X1F, 0X00, //打印中2， 代码 0x00
    0X00, 0X1F, 0X11, 0X11, 0X11, 0X11, 0X1F, 0X00, //缺纸，    代码 0x01
    0X00, 0X11, 0X1F, 0X11, 0X1F, 0X11, 0X1F, 0X00, //纸张就绪，代码 0x02
    0X00, 0X0A, 0X00, 0X11, 0X1F, 0X11, 0X1F, 0X00, //打印闲置，代码 0x03
    0X15, 0X0E, 0X04, 0X11, 0X1F, 0X11, 0X1F, 0X00, //打印中，  代码 0x04
    0x18, 0x18, 0x07, 0x08, 0x08, 0x08, 0x07, 0x00, //℃，       代码 0x05
    0X1D, 0X09, 0X09, 0X01, 0X15, 0X1C, 0X15, 0X00, //温度过高，代码 0x06
    0X1C, 0X08, 0X08, 0X01, 0X12, 0X0C, 0X08, 0X00  //温度正常，代码 0x07
};

void CgramWrite (void) {	// 装入CGRAM //
    unsigned char i;
    LCM2402_WriteCMD (0x06);			// CGRAM地址自动加1
    LCM2402_WriteCMD (0x40);			// CGRAM地址设为00处
    for (i = 0; i < 64; i++) {
        LCM2402_WriteData (Xword[i]); // 按数组写入数据
    }
}
/********************************************************************************************
// LCM2402初始化 //（使用者可自定义，加 * 号程序行必须保留但可修改）
/********************************************************************************************/
void LCM2402_Init (void) {
    LCM2402_WriteCMD (CMD_set82);	//* 显示模式设置：显示2行，每个字符为5*7个像素
    LCM2402_WriteCMD (CMD_clear);	//  显示清屏
    LCM2402_WriteCMD (CMD_back);		//* 数据指针指向第1行第1个字符位置
    LCM2402_WriteCMD (CMD_add1);		//  显示光标移动设置：文字不动，光标右移
    LCM2402_WriteCMD (CMD_dis_gb3); 	//  显示开及光标设置：显示开，光标开，闪烁开
    CgramWrite();					//  向CGRAM写入自定义字符
	LCM2402_GRAM_Init();
    GRAM[16] = 0;
}


/********************************************************************************************
// LCM2402清屏
/********************************************************************************************/
void LCM2402_CLS() {
    LCM2402_WriteCMD (CMD_clear);	//  显示清屏
    LCM2402_WriteCMD (CMD_back);		//* 数据指针指向第1行第1个字符位置
}


//void LCM2402_PrintGRAM() {
//    LCM2402_CLS();
//    LCM2402_Print (0x00, GRAM[0]);
//    LCM2402_Print (0x40, GRAM[1]);
//}