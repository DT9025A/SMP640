using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Management;

namespace SMP640Host
{
    public partial class MainForm : Form
    {
        private byte[] rBuf;
        private Boolean dataRecv = false;
        private Font font;
        Graphics gra;
        Bitmap Bitmap;

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

        delegate void SetTextCallBack(String str, String prefix);

        public MainForm()
        {
            InitializeComponent();
        }

        private static void Delay(int milliSecond)
        {
            int start = Environment.TickCount;
            while (Math.Abs(Environment.TickCount - start) < milliSecond)//毫秒
            {
                Application.DoEvents();//可执行某无聊的操作
            }
        }

        private Boolean WaitSerialReply()
        {
            int count = 0;
            while (!dataRecv && rBuf == null)
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

        private Boolean SendCommand(byte command, byte[] data)
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
            //StatusOut("包头 (3A) 等待回应");
            if (!WaitSerialReply())
            {
                StatusOut("超时");
                return false;
            }
            if (rBuf[0] == PACKET_REPLY_TRANS_START)
            {
                //StatusIn("开始传输 (01)");
            }
            else
            {
                StatusIn("应答错误 ({0})", ByteToHex(rBuf[0]));
                return false;
            }

            //数据长度
            buf[0] = (byte)(1 + data.Length);
            serialPort1.Write(buf, 0, 1);
            //StatusOut("数据长度 ({0}) 等待回应", ByteToHex(buf[0]));
            if (WaitSerialReply() == false)
            {
                StatusOut("超时");
                return false;
            }
            if (rBuf[0] == PACKET_REPLY_DATA_LENGTH_RECV)
            {
                //StatusIn("从机确认数据长度 (02), 数据长度: {0}", ByteToHex(rBuf[1]));
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
            //StatusOut("控制字 ({0}) 等待回应", ByteToHex(buf[0]));
            if (WaitSerialReply() == false)
            {
                StatusOut("超时");
                return false;
            }
            if (rBuf[0] == PACKET_REPLY_CMD_RECV)
            {
                //StatusIn("从机确认控制字 (03), 控制字: {0}", Convert.ToString(rBuf[1], 16));
            }
            else
            {
                StatusIn("应答错误 ({0})", Convert.ToString(rBuf[0], 16));
                return false;
            }

            //数据
            foreach (byte b in data)
            {
                buf[0] = b;
                checksum += b;
                serialPort1.Write(buf, 0, 1);
                //StatusOut("数据 ({0}) 等待回应", ByteToHex(buf[0]));
                if (WaitSerialReply() == false)
                {
                    StatusOut("超时");
                    return false;
                }
                if (rBuf[0] == PACKET_REPLY_CMD_EXECUTE)
                {
                    //StatusIn("从机确认数据 (04), 控制字: {0}, 数据: {1}", ByteToHex(rBuf[1]), ByteToHex(rBuf[2]));
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
            //StatusOut("校验和 ({0}) 等待回应", ByteToHex(buf[0]));
            if (WaitSerialReply() == false)
            {
                StatusOut("超时");
                return false;
            }
            if (rBuf[0] == PACKET_REPLY_CHECKSUM_CALC)
            {
                if (buf[0] != rBuf[1])
                {
                    //StatusIn("从机确认校验和 (05), 主机校验和: {0}, 从机校验和: {1}, 结果: {2}", ByteToHex(buf[0]), ByteToHex(rBuf[1]), buf[0] == rBuf[1] ? "一致" : "不一致");
                    StatusIn("校验和不一致 ({0} {1}), 通信可能无效", buf[0], rBuf[1]);
                }
            }
            else
            {
                StatusIn("应答错误 ({0})", ByteToHex(rBuf[0]));
                return false;
            }

            //传输结束
            buf[0] = PACKET_END;
            serialPort1.Write(buf, 0, 1);
            //StatusOut("包尾 (AA) 等待回应");
            if (WaitSerialReply() == false)
            {
                StatusOut("超时");
                return false;
            }
            if (rBuf[0] == PACKET_REPLY_TRANS_END)
            {
                //StatusIn("结束传输 (06)");
            }
            else
            {
                StatusIn("应答错误 ({0})", ByteToHex(rBuf[0]));
                return false;
            }
            StatusOut("操作成功");
            return true;
        }

/*        private String ByteArrayToHex(byte[] arr)
        {
            StringBuilder stringBuilder = new StringBuilder();
            foreach(byte b in arr)
            {
                stringBuilder.Append(Convert.ToString(b, 16).ToUpper());
                stringBuilder.Append(' ');
            }
            return stringBuilder.ToString(0, stringBuilder.Length - 1);
        }*/

        private void EnumCOMPorts()
        {
            comboBox_COM.Items.Clear();
            var search = new ManagementObjectSearcher("SELECT * FROM Win32_PnPEntity");
            foreach (ManagementObject hardInfo in search.Get())
            {
                if (hardInfo.Properties["Name"].Value != null && hardInfo.Properties["Name"].Value.ToString().Contains("(COM"))
                {
                    String strComName = hardInfo.Properties["Name"].Value.ToString();
                    comboBox_COM.Items.Add(strComName);
                }
            }
            if (comboBox_COM.Items.Count == 0)
            {
                comboBox_COM.Items.Add("无可用端口");
            }
            comboBox_COM.SelectedIndex = 0;
        }

        private void StatusData(String str, String prefix)
        {
            if (this.textBox1.InvokeRequired)
            {
                SetTextCallBack stcb = new SetTextCallBack(StatusData);
                this.Invoke(stcb, new object[] { str, prefix });
            }
            else
            {
                this.textBox1.AppendText(String.Format("{0}[{1}] {2}\r\n", DateTime.Now, prefix, str));
            }
        }

        private void StatusIn(String format, params object[] args)
        {
            StatusData(String.Format(format, args), "收");
        }

        private void StatusOut(String format, params object[] args)
        {
            StatusData(String.Format(format, args), "发");
        }

        private void StatusSelf(String format, params object[] args)
        {
            StatusData(String.Format(format, args), "信息");
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            EnumCOMPorts();
            comboBox_darkness.SelectedIndex = 4;
            gra = this.pictureBox1.CreateGraphics();
            Bitmap = new Bitmap(pictureBox1.Width, pictureBox1.Height);
        }

        private void Button_ScanCOM_Click(object sender, EventArgs e)
        {
            EnumCOMPorts();
        }

        private String ByteToHex(byte b)
        {
            return Convert.ToString(b, 16).ToUpper();
        }

        private void Button_OpenClose_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                serialPort1.Close();
                button_OpenClose.Text = "打开";
                StatusSelf("端口已关闭");
            }
            else
            {
                String COM = comboBox_COM.SelectedItem.ToString();
                serialPort1.BaudRate = 115200;
                serialPort1.Parity = System.IO.Ports.Parity.None;
                serialPort1.PortName = COM.Substring(COM.LastIndexOf("(") + 1).Replace(")", "");
                try { serialPort1.Open(); }
                catch(Exception) { }
                if (serialPort1.IsOpen)
                {
                    button_OpenClose.Text = "关闭";
                    StatusSelf("端口已打开 : {0}", COM);
                }
                else
                {
                    StatusSelf("端口打开失败 : {0}", COM);
                }
            }
        }

        private void SerialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            if (e.EventType != System.IO.Ports.SerialData.Chars)
            {
                return;
            }

            if (serialPort1.BytesToRead <= 0)
            {
                return;
            }

            rBuf = new byte[serialPort1.BytesToRead];

            serialPort1.Read(rBuf, 0, rBuf.Length);
            if (rBuf[0] == 0 && rBuf.Length > 1)
            {
                rBuf.CopyTo(rBuf, 1);
            }
            else if(Char.IsDigit((char)rBuf[0]) || Char.IsLetter((char)rBuf[0]) || Char.IsPunctuation((char)rBuf[0]))
            {
                StatusIn(Encoding.Default.GetString(rBuf).Trim());
            }
            else
            {
                dataRecv = true;
            }
        }

        private byte[] CopyArrayFrom(byte[] src, int start)
        {
            byte[] vs = new byte[src.Length - start];
            for (int i = 0; start < src.Length; start++, i++)
            {
                vs[i] = src[start];
            }
            return vs;
        }

        private void Button_TestTransfer_Click(object sender, EventArgs e)
        {
            Button_ClearChecksumError_Click(null, null);
        }

        private void Button_ClearChecksumError_Click(object sender, EventArgs e)
        {
            StatusOut("清除校验和错误");
            SendCommand(PACKET_CMD_CLC, new byte[] { 0 });
        }

        private void Button_StartPrint_Click(object sender, EventArgs e)
        {
            StatusOut("开始打印");
            SendCommand(PACKET_CMD_PRT, new byte[] { 0 });
            //label_IsPrinting.Text = "打印状态:正在打印";
        }

        private void Button_StopPrint_Click(object sender, EventArgs e)
        {
            StatusOut("停止打印");
            SendCommand(PACKET_CMD_STOP, new byte[] { 0 });
            //label_IsPrinting.Text = "打印状态:未在打印";
        }
        private void Button_ClearRecv_Click(object sender, EventArgs e)
        {
            textBox1.Text = "";
        }

        private void Button_setPrintDarkness_Click(object sender, EventArgs e)
        {
            StatusOut("设置打印灰度: {0}", comboBox_darkness.SelectedIndex + 1);
            SendCommand(PACKET_CMD_DARK, new byte[] { (byte)(comboBox_darkness.SelectedIndex + 1) });
        }

        private void Button_PRAM_Click(object sender, EventArgs e)
        {
            if (textBox_PRAM.Text.Length > 0)
            {
                int pos = Convert.ToInt32(textBox_PRAM.Text);
                StatusOut("设置PRAM位置下标: {0}", pos);
                SendCommand(PACKET_CMD_POS, new byte[] { (byte)(pos >> 8), (byte)(pos & 0xff) });
            }
        }

        private void Button_ClearPRAM_Click(object sender, EventArgs e)
        {
            StatusOut("设置PRAM位置下标: 0");
            SendCommand(PACKET_CMD_POS, new byte[] { 0, 0 });
            byte[] data = Enumerable.Repeat((byte)0x00, 120).ToArray();
            for (int i = 0; i < 4; i++)
            {
                StatusOut("传输数据, 长度: {0}", data.Length);
                SendCommand(PACKET_CMD_DATA, data);
            }
            StatusOut("设置PRAM位置下标: 0");
            SendCommand(PACKET_CMD_POS, new byte[] { 0, 0 });
        }

        private void button_SelectFont_Click(object sender, EventArgs e)
        {
            if(fontDialog1.ShowDialog() == DialogResult.OK)
            {
                font = fontDialog1.Font;
                label_Font.Text = String.Format("字体: {0}, {1}px", font.Name, font.SizeInPoints);
            }
        }


        private int GetHeight(int y)
        {
            int x;
            Boolean isAllWhite = true;
            for (; y < Bitmap.Height; y++)
            {
                for (x = 0; x < Bitmap.Width; x++)
                    if (Bitmap.GetPixel(x, y) != Color.White)
                    {
                        isAllWhite = false;
                        break;
                    }
                if (isAllWhite)
                    break;
                isAllWhite = true;
            }
            return y;
        }

        private void button_PrintText_Click(object sender, EventArgs e)
        {
            int x, y;
            StringBuilder stringBuilder = new StringBuilder();
            pictureBox1.DrawToBitmap(Bitmap, pictureBox1.ClientRectangle);
            Bitmap.Save("0.bmp");
            StatusSelf("{0}", GetHeight(2));
            /*for (y = 1; y < Bitmap.Height; y++)
            {
                for (x = 1; x < Bitmap.Width; x++)
                   stringBuilder.Append(Bitmap.GetPixel(x,y).ToArgb() != 0 ? 1 : 0);
            }
            StatusSelf(stringBuilder.ToString());*/
        }

        private void textBox_PrintText_TextChanged(object sender, EventArgs e)
        {
            gra.FillRectangle(Brushes.White, 0, 0, 384, 424);
            Brush bush = new SolidBrush(Color.Black);//填充的颜色
            if (textBox_PrintText.Text != null && font != null)
                gra.DrawString(textBox_PrintText.Text, font, bush, 0, 0);
        }
    }
}
