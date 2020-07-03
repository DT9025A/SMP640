#ifndef _UART_H_
#define _UART_H_

#include <STC12C5A60S2.H>

//通信协议定义
#define PACKET_HEAD     0x3A  //包头
#define PACKET_END      0xAA  //包尾
#define PACKET_CMD_DATA 0x00  //数据传输
#define PACKET_CMD_POS  0x01  //设置起始位置
#define PACKET_CMD_PRT  0x02  //开始打印
#define PACKET_CMD_STOP 0x03  //停止打印
#define PACKET_CMD_DARK 0x04  //设置灰度
#define PACKET_CMD_CLC  0x05  //清除校验和错误标志位

//收到一个字节后向上位机返回,收到这些数据上位机继续发送数据,防止数据连续发送造成数据丢失
#define PACKET_REPLY_TRANS_START      0x01   //收到包头,开始传输数据 (1字节)
#define PACKET_REPLY_DATA_LENGTH_RECV 0x02	 //收到数据长度,之后返回收到的数据长度 (2字节)
#define PACKET_REPLY_CMD_RECV         0x03   //收到控制字,之后返回收到的控制字 (2字节)
#define PACKET_REPLY_CMD_EXECUTE      0x04   //执行控制字,返回控制字和1字节执行结果 (3字节)
#define PACKET_REPLY_CHECKSUM_CALC    0x05   //计算校验和,返回校验和计算结果:0校验和正确,1校验和错误 (2字节)
#define PACKET_REPLY_TRANS_END        0x06   //收到包尾,传输结束 (1字节)

void UART_Init (void);
void UART_SendString (char *s);
void UART_SendData (unsigned char dat);

#endif