#include "uart.h"

extern bit isOnPrinting;			  //是否在打印
extern unsigned char xdata PRAM[480]; //打印数据
extern unsigned char PDARKNESS;		  //打印灰度
//extern unsigned int PRAMAvaliable;//行数据是否可用 xxxx xx[10][9] [8][7][6][5] [4][3][2][1]
//extern unsigned int PRLINE;
//unsigned char xdata URRAM[10];	  //UART接收缓冲区
//unsigned char URPTR = 0;			  //UART接收数据下标
unsigned char URCHKSUM = 0;           //UART校验和
unsigned char URCTRL = 0xFF;          //UART接收的控制字
unsigned char URDATANUM = 0;	      //收到数据数量
unsigned char URDATATOTAL = 0;		  //要接收的数据总量
unsigned int PDATAPTR = 0;			  //PRAM操作数据下标

bit isRecvingData = 0;				  //是否在接收数据
bit isRecvPacketHead = 0;             //是否接收到包头
bit isRecvedHSBPPTR = 0;			  //是否接收到PDATAPTR的高八位
bit isChecksumError = 0;			  //是否为错误的校验和

void Timer0_Init (void);

void Uart_Init (void) {	//115200bps@11.0592MHz
    PCON |= 0x80;		//使能波特率倍速位SMOD
    SCON = 0x50;		//8位数据,可变波特率
    AUXR |= 0x04;		//独立波特率发生器时钟为Fosc,即1T
    BRT = 0xFA;         //设定独立波特率发生器重装值
    AUXR |= 0x01;		//串口1选择独立波特率发生器为波特率发生器
    AUXR |= 0x10;		//启动独立波特率发生器
    ES = 1;             // 打开接收中断
    PDATAPTR = 0;
}

void UART_SendString (char *s) {
    while (*s) {
        UART_SendData (*s++);
    }
}

void UART_SendData (unsigned char dat) {
    SBUF = dat;
    while (TI == 0);
    TI = 0;
}

/************************************
	UART 中断服务函数

	协议定义:
	包头[1]{3A} 数据长度[1]{0-FF}(从控制字到数据结束) 控制字[1]{0-5} 数据 校验和[1]{0-FF}(从控制字到数据结束) 包尾[1]{AA}
	3A 02 05 00 05 AA
	版本2 :只回复收到包, 不回复数据包
	控制字: 00 数据	 [...]
			01 设置下一次数据传送在PRAM的起始位置  [HSB LSB]
			02 开始打印  [RAM行数]
			03 停止打印	 [00]
			04 设置打印灰度 [1-10]
*************************************/
void UART_ISR() interrupt 4 using 1 {
    if (RI) {
        RI = 0;
        //接收到包头
        if (SBUF == PACKET_HEAD && isRecvingData == 0) {
            isRecvPacketHead = 1; //收到包头标志
            isRecvingData = 1;	  //接收数据标志
            //URPTR = 0;		  //接收下标回零
            URCHKSUM = 0;		  //校验和回零
            URDATANUM = 0;		  //接收数据量回零
            URDATATOTAL = 0;	  //总数据量回零
            URCTRL = 0xFF;		  //控制字回无效
            TR0 = 0;		      //关T0
            UART_SendData (PACKET_REPLY_TRANS_START);
            return;
        }
        //接收数据长度
        if (isRecvPacketHead) {
            isRecvPacketHead = 0; //收到包头标志回零
            URDATATOTAL = SBUF;	  //置数据长度
            UART_SendData (PACKET_REPLY_DATA_LENGTH_RECV);
            return;
        }
        //接收控制字
        if (URDATATOTAL != 0 && URCTRL == 0xFF) {
            URCTRL = SBUF;		  //置控制字
            URCHKSUM = SBUF;	  //置校验和第一字节
            URDATANUM = 1;		  //置数据量
            UART_SendData (PACKET_REPLY_CMD_RECV);
            return;
        }
        //判断控制字
        if (URCTRL != 0xFF && isRecvingData == 1 && URDATANUM != URDATATOTAL) {
            URCHKSUM += SBUF;				  //校验和
            URDATANUM += 1;				  	  //数据量
            if (URCTRL == PACKET_CMD_DATA) {  //数据传输
                PRAM[PDATAPTR++] = SBUF;	  //收数据
                goto CMD_OUT;
            }
            if (URCTRL == PACKET_CMD_POS) {	  //设置下标
                if (isRecvedHSBPPTR) {
                    PDATAPTR |= SBUF;	      //收低八位
                    isRecvedHSBPPTR = 0;
                } else {
                    PDATAPTR = SBUF << 8;	  //收高八位
                    isRecvedHSBPPTR = 1;
                }
                goto CMD_OUT;
            }
            if (URCTRL == PACKET_CMD_PRT) {   //开始打印
                isOnPrinting = 1;	          //收数据
                goto CMD_OUT;
            }
            if (URCTRL == PACKET_CMD_STOP) {  //停止打印
                isOnPrinting = 0;	          //收数据
                goto CMD_OUT;
            }
            if (URCTRL == PACKET_CMD_DARK) {  //设置灰度
                PDARKNESS = SBUF;	          //收数据
                goto CMD_OUT;
            }
            if (URCTRL == PACKET_CMD_CLC) {	  //清除校验和错误标志位
                //什么都不做就行
                //一次正常的通信就可以清除该标志位
                goto CMD_OUT;
            }
        }
        //数据长度达到传送值
        if (URDATANUM == URDATATOTAL && isRecvingData == 1) {		 //数据接收完毕
            if (URCHKSUM != SBUF) {									 //比对校验和
                isChecksumError = 1;								 //不同,置校验和错误标志位
            }
            isRecvingData = 0;										 //不再接收数据
            UART_SendData (PACKET_REPLY_CHECKSUM_CALC);
            UART_SendData (URCHKSUM);
            return;
        }
        //接收到包尾
        if (SBUF == PACKET_END && isRecvingData == 0) {
            URCTRL = 0xFF;
            UART_SendData (PACKET_REPLY_TRANS_END);
            Timer0_Init();
            return;
        }
        return;
		//CMD5
CMD_OUT:
		UART_SendData (PACKET_REPLY_CMD_EXECUTE);
		return;
    }
}