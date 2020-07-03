using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace SMP640Host
{
    class UARTProtocol
    {
        private readonly byte PACKET_HEAD = 0x3A;      //包头
        private readonly byte PACKET_END = 0xAA;       //包尾
        private readonly byte PACKET_CMD_DATA = 0x00;  //数据传输
        private readonly byte PACKET_CMD_POS = 0x01;   //设置起始位置
        private readonly byte PACKET_CMD_PRT = 0x02;   //开始打印
        private readonly byte PACKET_CMD_STOP = 0x03;  //停止打印
        private readonly byte PACKET_CMD_DARK = 0x04;  //设置灰度
        private readonly byte PACKET_CMD_CLC = 0x05;   //清除校验和错误标志位

        //收到一个字节后向上位机返回,收到这些数据上位机继续发送数据,防止数据连续发送造成数据丢失
        private readonly byte PACKET_REPLY_TRANS_START = 0x01;        //收到包头,开始传输数据 (1字节)
        private readonly byte PACKET_REPLY_DATA_LENGTH_RECV = 0x02;	 //收到数据长度,之后返回收到的数据长度 (2字节)
        private readonly byte PACKET_REPLY_CMD_RECV = 0x03;           //收到控制字,之后返回收到的控制字 (2字节)
        private readonly byte PACKET_REPLY_CMD_EXECUTE = 0x04;        //执行控制字,返回控制字和1字节执行结果 (3字节)
        private readonly byte PACKET_REPLY_CHECKSUM_CALC = 0x05;      //计算校验和,返回校验和计算结果:0校验和正确,1校验和错误 (2字节)
        private readonly byte PACKET_REPLY_TRANS_END = 0x06;          //收到包尾,传输结束 (1字节)

        public byte[] rBuf;
        public Boolean dataRecv = false;

        private static void Delay(int milliSecond)
        {
            int start = Environment.TickCount;
            while (Math.Abs(Environment.TickCount - start) < milliSecond)//毫秒
            {
                Application.DoEvents();//可执行某无聊的操作
            }
        }

        public void CallBackFunc(byte[] b)
        {
            rBuf = b;
            dataRecv = true;
        }

        private Boolean WaitSerialReply()
        {
            int count = 0;
            while (!dataRecv)
            {
                Delay(1);
                count += 1;
                if (count == 295)
                {
                    return false;
                }
            }
            dataRecv = false;
            return true;
        }

        private String ByteToHex(byte b)
        {
            return Convert.ToString(b, 16).ToUpper();
        }

        public Boolean SendCommand(byte command, byte[] data)
        {
            int checksum = 0;
            //优化去他妈
            byte[] buf = new byte[1];

            if (serialPort1.IsOpen == false)
            {
                StatusSelf("端口未打开");
                return false;
            }
            //包头
            buf[0] = PACKET_HEAD;
            serialPort1.Write(buf, 0, 1);
            StatusOut("包头 (3A) 等待回应");
            if (!WaitSerialReply())
            {
                StatusOut("超时");
                return false;
            }
            if (rBuf[0] == PACKET_REPLY_TRANS_START)
            {
                StatusIn("开始传输 (01)");
            }
            else
            {
                StatusIn("应答错误 ({0})", ByteToHex(rBuf[0]));
                return false;
            }

            //数据长度
            buf[0] = (byte)(1 + data.Length);
            serialPort1.Write(buf, 0, 1);
            StatusOut("数据长度 ({0}) 等待回应", ByteToHex(buf[0]));
            if (WaitSerialReply() == false)
            {
                StatusOut("超时");
                return false;
            }
            if (rBuf[0] == PACKET_REPLY_DATA_LENGTH_RECV)
            {
                StatusIn("从机确认数据长度 (02), 数据长度: {0}", ByteToHex(rBuf[1]));
            }
            else
            {
                StatusIn("应答错误 ({0})", ByteToHex(rBuf[0]));
                return false;
            }

            //控制字
            buf[0] = command;
            checksum += command;
            serialPort1.Write(buf, 0, 1);
            StatusOut("控制字 ({0}) 等待回应", ByteToHex(buf[0]));
            if (WaitSerialReply() == false)
            {
                StatusOut("超时");
                return false;
            }
            if (rBuf[0] == PACKET_REPLY_CMD_RECV)
            {
                StatusIn("从机确认控制字 (03), 控制字: {0}", ByteToHex(rBuf[1]));
            }
            else
            {
                StatusIn("应答错误 ({0})", ByteToHex(rBuf[0]));
                return false;
            }

            //数据
            foreach (byte b in data)
            {
                buf[0] = b;
                checksum += b;
                serialPort1.Write(buf, 0, 1);
                StatusOut("数据 ({0}) 等待回应", ByteToHex(buf[0]));
                if (WaitSerialReply() == false)
                {
                    StatusOut("超时");
                    return false;
                }
                if (rBuf[0] == PACKET_REPLY_CMD_EXECUTE)
                {
                    StatusIn("从机确认数据 (04), 控制字: {0}, 数据: {1}", ByteToHex(rBuf[1]), ByteToHex(rBuf[2]));
                }
                else
                {
                    StatusIn("应答错误 ({0})", ByteToHex(rBuf[0]));
                    return false;
                }
            }

            //校验和
            buf[0] = (byte)(checksum % 256);
            serialPort1.Write(buf, 0, 1);
            StatusOut("校验和 ({0}) 等待回应", ByteToHex(buf[0]));
            if (WaitSerialReply() == false)
            {
                StatusOut("超时");
                return false;
            }
            if (rBuf[0] == PACKET_REPLY_CHECKSUM_CALC)
            {
                StatusIn("从机确认校验和 (05), 主机校验和: {0}, 从机校验和: {1}, 结果: {2}", ByteToHex(buf[0]), ByteToHex(rBuf[1]), buf[0] == rBuf[1] ? "一致" : "不一致");
            }
            else
            {
                StatusIn("应答错误 ({0})", ByteToHex(rBuf[0]));
                return false;
            }

            //传输结束
            buf[0] = PACKET_END;
            serialPort1.Write(buf, 0, 1);
            StatusOut("包尾 (AA) 等待回应");
            if (WaitSerialReply() == false)
            {
                StatusOut("超时");
                return false;
            }
            if (rBuf[0] == PACKET_REPLY_TRANS_END)
            {
                StatusIn("结束传输 (06)");
            }
            else
            {
                StatusIn("应答错误 ({0})", ByteToHex(rBuf[0]));
                return false;
            }
            return true;
        }
    }
}
