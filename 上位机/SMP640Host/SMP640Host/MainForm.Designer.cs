namespace SMP640Host
{
    public partial class MainForm
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.comboBox_COM = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.button_ScanCOM = new System.Windows.Forms.Button();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.button_OpenClose = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.button_ClearPRAM = new System.Windows.Forms.Button();
            this.button_PRAM = new System.Windows.Forms.Button();
            this.textBox_PRAM = new System.Windows.Forms.TextBox();
            this.button_setPrintDarkness = new System.Windows.Forms.Button();
            this.comboBox_darkness = new System.Windows.Forms.ComboBox();
            this.button_TestTransfer = new System.Windows.Forms.Button();
            this.button_ClearChecksumError = new System.Windows.Forms.Button();
            this.button_StopPrint = new System.Windows.Forms.Button();
            this.button_StartPrint = new System.Windows.Forms.Button();
            this.button_ClearRecv = new System.Windows.Forms.Button();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.textBox_PrintText = new System.Windows.Forms.TextBox();
            this.button_PrintText = new System.Windows.Forms.Button();
            this.fontDialog1 = new System.Windows.Forms.FontDialog();
            this.button_SelectFont = new System.Windows.Forms.Button();
            this.label_Font = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // comboBox_COM
            // 
            this.comboBox_COM.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_COM.FormattingEnabled = true;
            this.comboBox_COM.Location = new System.Drawing.Point(47, 11);
            this.comboBox_COM.Name = "comboBox_COM";
            this.comboBox_COM.Size = new System.Drawing.Size(206, 20);
            this.comboBox_COM.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 14);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 12);
            this.label1.TabIndex = 1;
            this.label1.Text = "端口";
            // 
            // serialPort1
            // 
            this.serialPort1.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.SerialPort1_DataReceived);
            // 
            // button_ScanCOM
            // 
            this.button_ScanCOM.Location = new System.Drawing.Point(259, 11);
            this.button_ScanCOM.Name = "button_ScanCOM";
            this.button_ScanCOM.Size = new System.Drawing.Size(63, 21);
            this.button_ScanCOM.TabIndex = 2;
            this.button_ScanCOM.Text = "扫描";
            this.button_ScanCOM.UseVisualStyleBackColor = true;
            this.button_ScanCOM.Click += new System.EventHandler(this.Button_ScanCOM_Click);
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(14, 295);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBox1.Size = new System.Drawing.Size(376, 104);
            this.textBox1.TabIndex = 3;
            // 
            // button_OpenClose
            // 
            this.button_OpenClose.Location = new System.Drawing.Point(328, 11);
            this.button_OpenClose.Name = "button_OpenClose";
            this.button_OpenClose.Size = new System.Drawing.Size(63, 21);
            this.button_OpenClose.TabIndex = 4;
            this.button_OpenClose.Text = "打开";
            this.button_OpenClose.UseVisualStyleBackColor = true;
            this.button_OpenClose.Click += new System.EventHandler(this.Button_OpenClose_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label_Font);
            this.groupBox1.Controls.Add(this.button_SelectFont);
            this.groupBox1.Controls.Add(this.button_PrintText);
            this.groupBox1.Controls.Add(this.textBox_PrintText);
            this.groupBox1.Controls.Add(this.button_ClearPRAM);
            this.groupBox1.Controls.Add(this.button_PRAM);
            this.groupBox1.Controls.Add(this.textBox_PRAM);
            this.groupBox1.Controls.Add(this.button_setPrintDarkness);
            this.groupBox1.Controls.Add(this.comboBox_darkness);
            this.groupBox1.Controls.Add(this.button_TestTransfer);
            this.groupBox1.Controls.Add(this.button_ClearChecksumError);
            this.groupBox1.Controls.Add(this.button_StopPrint);
            this.groupBox1.Controls.Add(this.button_StartPrint);
            this.groupBox1.Location = new System.Drawing.Point(14, 38);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(376, 251);
            this.groupBox1.TabIndex = 5;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "打印机操作";
            // 
            // button_ClearPRAM
            // 
            this.button_ClearPRAM.Location = new System.Drawing.Point(13, 87);
            this.button_ClearPRAM.Name = "button_ClearPRAM";
            this.button_ClearPRAM.Size = new System.Drawing.Size(69, 22);
            this.button_ClearPRAM.TabIndex = 8;
            this.button_ClearPRAM.Text = "清空PRAM";
            this.button_ClearPRAM.UseVisualStyleBackColor = true;
            this.button_ClearPRAM.Click += new System.EventHandler(this.Button_ClearPRAM_Click);
            // 
            // button_PRAM
            // 
            this.button_PRAM.Location = new System.Drawing.Point(286, 59);
            this.button_PRAM.Name = "button_PRAM";
            this.button_PRAM.Size = new System.Drawing.Size(75, 20);
            this.button_PRAM.TabIndex = 7;
            this.button_PRAM.Text = "置PRAM位置";
            this.button_PRAM.UseVisualStyleBackColor = true;
            this.button_PRAM.Click += new System.EventHandler(this.Button_PRAM_Click);
            // 
            // textBox_PRAM
            // 
            this.textBox_PRAM.Location = new System.Drawing.Point(164, 59);
            this.textBox_PRAM.Name = "textBox_PRAM";
            this.textBox_PRAM.Size = new System.Drawing.Size(114, 21);
            this.textBox_PRAM.TabIndex = 6;
            // 
            // button_setPrintDarkness
            // 
            this.button_setPrintDarkness.Location = new System.Drawing.Point(92, 60);
            this.button_setPrintDarkness.Name = "button_setPrintDarkness";
            this.button_setPrintDarkness.Size = new System.Drawing.Size(65, 20);
            this.button_setPrintDarkness.TabIndex = 5;
            this.button_setPrintDarkness.Text = "设置灰度";
            this.button_setPrintDarkness.UseVisualStyleBackColor = true;
            this.button_setPrintDarkness.Click += new System.EventHandler(this.Button_setPrintDarkness_Click);
            // 
            // comboBox_darkness
            // 
            this.comboBox_darkness.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBox_darkness.FormattingEnabled = true;
            this.comboBox_darkness.Items.AddRange(new object[] {
            "1",
            "2",
            "3",
            "4",
            "5",
            "6",
            "7",
            "8",
            "9",
            "10"});
            this.comboBox_darkness.Location = new System.Drawing.Point(13, 60);
            this.comboBox_darkness.Name = "comboBox_darkness";
            this.comboBox_darkness.Size = new System.Drawing.Size(68, 20);
            this.comboBox_darkness.TabIndex = 4;
            // 
            // button_TestTransfer
            // 
            this.button_TestTransfer.Location = new System.Drawing.Point(284, 26);
            this.button_TestTransfer.Name = "button_TestTransfer";
            this.button_TestTransfer.Size = new System.Drawing.Size(78, 24);
            this.button_TestTransfer.TabIndex = 3;
            this.button_TestTransfer.Text = "测试通信";
            this.button_TestTransfer.UseVisualStyleBackColor = true;
            this.button_TestTransfer.Click += new System.EventHandler(this.Button_TestTransfer_Click);
            // 
            // button_ClearChecksumError
            // 
            this.button_ClearChecksumError.Location = new System.Drawing.Point(164, 26);
            this.button_ClearChecksumError.Name = "button_ClearChecksumError";
            this.button_ClearChecksumError.Size = new System.Drawing.Size(114, 24);
            this.button_ClearChecksumError.TabIndex = 2;
            this.button_ClearChecksumError.Text = "清除校验和错误";
            this.button_ClearChecksumError.UseVisualStyleBackColor = true;
            this.button_ClearChecksumError.Click += new System.EventHandler(this.Button_ClearChecksumError_Click);
            // 
            // button_StopPrint
            // 
            this.button_StopPrint.Location = new System.Drawing.Point(90, 26);
            this.button_StopPrint.Name = "button_StopPrint";
            this.button_StopPrint.Size = new System.Drawing.Size(68, 24);
            this.button_StopPrint.TabIndex = 1;
            this.button_StopPrint.Text = "停止打印";
            this.button_StopPrint.UseVisualStyleBackColor = true;
            this.button_StopPrint.Click += new System.EventHandler(this.Button_StopPrint_Click);
            // 
            // button_StartPrint
            // 
            this.button_StartPrint.Location = new System.Drawing.Point(13, 26);
            this.button_StartPrint.Name = "button_StartPrint";
            this.button_StartPrint.Size = new System.Drawing.Size(69, 24);
            this.button_StartPrint.TabIndex = 0;
            this.button_StartPrint.Text = "开始打印";
            this.button_StartPrint.UseVisualStyleBackColor = true;
            this.button_StartPrint.Click += new System.EventHandler(this.Button_StartPrint_Click);
            // 
            // button_ClearRecv
            // 
            this.button_ClearRecv.Location = new System.Drawing.Point(14, 411);
            this.button_ClearRecv.Name = "button_ClearRecv";
            this.button_ClearRecv.Size = new System.Drawing.Size(376, 24);
            this.button_ClearRecv.TabIndex = 7;
            this.button_ClearRecv.Text = "清空接收区";
            this.button_ClearRecv.UseVisualStyleBackColor = true;
            this.button_ClearRecv.Click += new System.EventHandler(this.Button_ClearRecv_Click);
            // 
            // pictureBox1
            // 
            this.pictureBox1.Location = new System.Drawing.Point(407, 11);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(384, 424);
            this.pictureBox1.TabIndex = 8;
            this.pictureBox1.TabStop = false;
            // 
            // textBox_PrintText
            // 
            this.textBox_PrintText.Location = new System.Drawing.Point(13, 198);
            this.textBox_PrintText.Name = "textBox_PrintText";
            this.textBox_PrintText.Size = new System.Drawing.Size(348, 21);
            this.textBox_PrintText.TabIndex = 9;
            this.textBox_PrintText.TextChanged += new System.EventHandler(this.textBox_PrintText_TextChanged);
            // 
            // button_PrintText
            // 
            this.button_PrintText.Location = new System.Drawing.Point(300, 221);
            this.button_PrintText.Name = "button_PrintText";
            this.button_PrintText.Size = new System.Drawing.Size(62, 24);
            this.button_PrintText.TabIndex = 10;
            this.button_PrintText.Text = "打印文字";
            this.button_PrintText.UseVisualStyleBackColor = true;
            this.button_PrintText.Click += new System.EventHandler(this.button_PrintText_Click);
            // 
            // button_SelectFont
            // 
            this.button_SelectFont.Location = new System.Drawing.Point(229, 221);
            this.button_SelectFont.Name = "button_SelectFont";
            this.button_SelectFont.Size = new System.Drawing.Size(68, 24);
            this.button_SelectFont.TabIndex = 11;
            this.button_SelectFont.Text = "选择字体";
            this.button_SelectFont.UseVisualStyleBackColor = true;
            this.button_SelectFont.Click += new System.EventHandler(this.button_SelectFont_Click);
            // 
            // label_Font
            // 
            this.label_Font.AutoSize = true;
            this.label_Font.Location = new System.Drawing.Point(11, 227);
            this.label_Font.Name = "label_Font";
            this.label_Font.Size = new System.Drawing.Size(35, 12);
            this.label_Font.TabIndex = 12;
            this.label_Font.Text = "字体:";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(809, 450);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.button_ClearRecv);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.button_OpenClose);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.button_ScanCOM);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.comboBox_COM);
            this.Name = "MainForm";
            this.Text = "SMP640上位机 DT9025A版本";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox comboBox_COM;
        private System.Windows.Forms.Label label1;
        public System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.Button button_ScanCOM;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Button button_OpenClose;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button button_StartPrint;
        private System.Windows.Forms.Button button_StopPrint;
        private System.Windows.Forms.Button button_TestTransfer;
        private System.Windows.Forms.Button button_ClearChecksumError;
        private System.Windows.Forms.Button button_ClearRecv;
        private System.Windows.Forms.ComboBox comboBox_darkness;
        private System.Windows.Forms.Button button_setPrintDarkness;
        private System.Windows.Forms.Button button_PRAM;
        private System.Windows.Forms.TextBox textBox_PRAM;
        private System.Windows.Forms.Button button_ClearPRAM;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Label label_Font;
        private System.Windows.Forms.Button button_SelectFont;
        private System.Windows.Forms.Button button_PrintText;
        private System.Windows.Forms.TextBox textBox_PrintText;
        private System.Windows.Forms.FontDialog fontDialog1;
    }
}

