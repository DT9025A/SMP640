#ifndef _UART_H_
#define _UART_H_

#include <STC12C5A60S2.H>

//ͨ��Э�鶨��
#define PACKET_HEAD     0x3A  //��ͷ
#define PACKET_END      0xAA  //��β
#define PACKET_CMD_DATA 0x00  //���ݴ���
#define PACKET_CMD_POS  0x01  //������ʼλ��
#define PACKET_CMD_PRT  0x02  //��ʼ��ӡ
#define PACKET_CMD_STOP 0x03  //ֹͣ��ӡ
#define PACKET_CMD_DARK 0x04  //���ûҶ�
#define PACKET_CMD_CLC  0x05  //���У��ʹ����־λ

//�յ�һ���ֽں�����λ������,�յ���Щ������λ��������������,��ֹ������������������ݶ�ʧ
#define PACKET_REPLY_TRANS_START      0x01   //�յ���ͷ,��ʼ�������� (1�ֽ�)
#define PACKET_REPLY_DATA_LENGTH_RECV 0x02	 //�յ����ݳ���,֮�󷵻��յ������ݳ��� (2�ֽ�)
#define PACKET_REPLY_CMD_RECV         0x03   //�յ�������,֮�󷵻��յ��Ŀ����� (2�ֽ�)
#define PACKET_REPLY_CMD_EXECUTE      0x04   //ִ�п�����,���ؿ����ֺ�1�ֽ�ִ�н�� (3�ֽ�)
#define PACKET_REPLY_CHECKSUM_CALC    0x05   //����У���,����У��ͼ�����:0У�����ȷ,1У��ʹ��� (2�ֽ�)
#define PACKET_REPLY_TRANS_END        0x06   //�յ���β,������� (1�ֽ�)

void UART_Init (void);
void UART_SendString (char *s);
void UART_SendData (unsigned char dat);

#endif