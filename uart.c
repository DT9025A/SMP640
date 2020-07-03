#include "uart.h"

extern bit isOnPrinting;			  //�Ƿ��ڴ�ӡ
extern unsigned char xdata PRAM[480]; //��ӡ����
extern unsigned char PDARKNESS;		  //��ӡ�Ҷ�
//extern unsigned int PRAMAvaliable;//�������Ƿ���� xxxx xx[10][9] [8][7][6][5] [4][3][2][1]
//extern unsigned int PRLINE;
//unsigned char xdata URRAM[10];	  //UART���ջ�����
//unsigned char URPTR = 0;			  //UART���������±�
unsigned char URCHKSUM = 0;           //UARTУ���
unsigned char URCTRL = 0xFF;          //UART���յĿ�����
unsigned char URDATANUM = 0;	      //�յ���������
unsigned char URDATATOTAL = 0;		  //Ҫ���յ���������
unsigned int PDATAPTR = 0;			  //PRAM���������±�

bit isRecvingData = 0;				  //�Ƿ��ڽ�������
bit isRecvPacketHead = 0;             //�Ƿ���յ���ͷ
bit isRecvedHSBPPTR = 0;			  //�Ƿ���յ�PDATAPTR�ĸ߰�λ
bit isChecksumError = 0;			  //�Ƿ�Ϊ�����У���

void Timer0_Init (void);

void Uart_Init (void) {	//115200bps@11.0592MHz
    PCON |= 0x80;		//ʹ�ܲ����ʱ���λSMOD
    SCON = 0x50;		//8λ����,�ɱ䲨����
    AUXR |= 0x04;		//���������ʷ�����ʱ��ΪFosc,��1T
    BRT = 0xFA;         //�趨���������ʷ�������װֵ
    AUXR |= 0x01;		//����1ѡ����������ʷ�����Ϊ�����ʷ�����
    AUXR |= 0x10;		//�������������ʷ�����
    ES = 1;             // �򿪽����ж�
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
	UART �жϷ�����

	Э�鶨��:
	��ͷ[1]{3A} ���ݳ���[1]{0-FF}(�ӿ����ֵ����ݽ���) ������[1]{0-5} ���� У���[1]{0-FF}(�ӿ����ֵ����ݽ���) ��β[1]{AA}
	3A 02 05 00 05 AA
	�汾2 :ֻ�ظ��յ���, ���ظ����ݰ�
	������: 00 ����	 [...]
			01 ������һ�����ݴ�����PRAM����ʼλ��  [HSB LSB]
			02 ��ʼ��ӡ  [RAM����]
			03 ֹͣ��ӡ	 [00]
			04 ���ô�ӡ�Ҷ� [1-10]
*************************************/
void UART_ISR() interrupt 4 using 1 {
    if (RI) {
        RI = 0;
        //���յ���ͷ
        if (SBUF == PACKET_HEAD && isRecvingData == 0) {
            isRecvPacketHead = 1; //�յ���ͷ��־
            isRecvingData = 1;	  //�������ݱ�־
            //URPTR = 0;		  //�����±����
            URCHKSUM = 0;		  //У��ͻ���
            URDATANUM = 0;		  //��������������
            URDATATOTAL = 0;	  //������������
            URCTRL = 0xFF;		  //�����ֻ���Ч
            TR0 = 0;		      //��T0
            UART_SendData (PACKET_REPLY_TRANS_START);
            return;
        }
        //�������ݳ���
        if (isRecvPacketHead) {
            isRecvPacketHead = 0; //�յ���ͷ��־����
            URDATATOTAL = SBUF;	  //�����ݳ���
            UART_SendData (PACKET_REPLY_DATA_LENGTH_RECV);
            return;
        }
        //���տ�����
        if (URDATATOTAL != 0 && URCTRL == 0xFF) {
            URCTRL = SBUF;		  //�ÿ�����
            URCHKSUM = SBUF;	  //��У��͵�һ�ֽ�
            URDATANUM = 1;		  //��������
            UART_SendData (PACKET_REPLY_CMD_RECV);
            return;
        }
        //�жϿ�����
        if (URCTRL != 0xFF && isRecvingData == 1 && URDATANUM != URDATATOTAL) {
            URCHKSUM += SBUF;				  //У���
            URDATANUM += 1;				  	  //������
            if (URCTRL == PACKET_CMD_DATA) {  //���ݴ���
                PRAM[PDATAPTR++] = SBUF;	  //������
                goto CMD_OUT;
            }
            if (URCTRL == PACKET_CMD_POS) {	  //�����±�
                if (isRecvedHSBPPTR) {
                    PDATAPTR |= SBUF;	      //�յͰ�λ
                    isRecvedHSBPPTR = 0;
                } else {
                    PDATAPTR = SBUF << 8;	  //�ո߰�λ
                    isRecvedHSBPPTR = 1;
                }
                goto CMD_OUT;
            }
            if (URCTRL == PACKET_CMD_PRT) {   //��ʼ��ӡ
                isOnPrinting = 1;	          //������
                goto CMD_OUT;
            }
            if (URCTRL == PACKET_CMD_STOP) {  //ֹͣ��ӡ
                isOnPrinting = 0;	          //������
                goto CMD_OUT;
            }
            if (URCTRL == PACKET_CMD_DARK) {  //���ûҶ�
                PDARKNESS = SBUF;	          //������
                goto CMD_OUT;
            }
            if (URCTRL == PACKET_CMD_CLC) {	  //���У��ʹ����־λ
                //ʲô����������
                //һ��������ͨ�žͿ�������ñ�־λ
                goto CMD_OUT;
            }
        }
        //���ݳ��ȴﵽ����ֵ
        if (URDATANUM == URDATATOTAL && isRecvingData == 1) {		 //���ݽ������
            if (URCHKSUM != SBUF) {									 //�ȶ�У���
                isChecksumError = 1;								 //��ͬ,��У��ʹ����־λ
            }
            isRecvingData = 0;										 //���ٽ�������
            UART_SendData (PACKET_REPLY_CHECKSUM_CALC);
            UART_SendData (URCHKSUM);
            return;
        }
        //���յ���β
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