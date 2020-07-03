#ifndef _1602_H_
#define _1602_H_

#include <STC12C5A60S2.H>
#include <string.h>

#define LCM2402_DB0_DB7 P0

sbit LCM2402_RS   = P2 ^ 6;					// 定义LCM2402的RS控制线
sbit LCM2402_RW   = P2 ^ 5;					// 定义LCM2402的RW控制线
sbit LCM2402_E    = P2 ^ 7;					// 定义LCM2402的E控制线
sbit LCM2402_Busy = LCM2402_DB0_DB7 ^ 7;	// 定义LCM2402的测忙线（与LCM2402_DB0_DB7关联）

#define CMD_clear	0x01             // 清除屏幕
#define CMD_back	0x02             // DDRAM回零位
#define CMD_dec1	0x04             // 读入后AC（指针）减1，向左写
#define CMD_add1	0x06             // 读入后AC（指针）加1，向右写
#define CMD_dis_gb1	0x0f             // 开显示_开光标_开光标闪烁
#define CMD_dis_gb2	0x0e             // 开显示_开光标_关光标闪烁
#define CMD_dis_gb3	0x0c             // 开显示_关光标_关光标闪烁
#define CMD_OFF_dis	0x08             // 关显示_关光标_关光标闪烁
#define CMD_set82	0x38             // 8位总线_2行显示
#define CMD_set81	0x30             // 8位总线_1行显示（上边行）
#define CMD_set42	0x28             // 4位总线_2行显示
#define CMD_set41	0x20             // 4位总线_1行显示（上边行）
#define lin_1       0x80             // 4位总线_1行显示（上边行）
#define lin_2       0xc0             // 4位总线_1行显示（上边行）

#define CHAR_C      0x05	 		 //℃，       代码 0x00
#define CHAR_PAPERR 0x01	 		 //缺纸，    代码 0x01
#define CHAR_PAPOK  0x02	 		 //纸张就绪，代码 0x02
#define CHAR_IDLE   0x03	 		 //打印闲置，代码 0x03
#define CHAR_PING1  0x04	 		 //打印中，  代码 0x04
#define CHAR_PING2  0x00	 		 //打印中2， 代码 0x05
#define CHAR_TEMPH  0x06	 		 //温度过高，代码 0x06
#define CHAR_TEMPOK 0x07	 		 //温度正常，代码 0x07


void LCM2402_TestBusy (void);
void LCM2402_WriteCMD (unsigned char LCM2402_command);
void LCM2402_WriteData (unsigned char LCM2402_data);
void LCM2402_Print (unsigned char a, unsigned char *str);
void LCM2402_PrintChar (unsigned char a, unsigned char t);
void LCM2402_CLS();
void LCM2402_Init (void);
//void LCM2402_PrintGRAM();
#define LCM2402_GRAM_Init() memset(GRAM,' ',17)

//1602显示缓冲区
extern char xdata GRAM[20];
//#define GRAML(L) (char*)&GRAM[L]

#endif