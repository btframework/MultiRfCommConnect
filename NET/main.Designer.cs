
namespace RfCommClientTest
{
    partial class fmMain
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.lbLog = new System.Windows.Forms.ListBox();
            this.btDiscover = new System.Windows.Forms.Button();
            this.cbUnpair = new System.Windows.Forms.CheckBox();
            this.btClear = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // lbLog
            // 
            this.lbLog.FormattingEnabled = true;
            this.lbLog.Location = new System.Drawing.Point(12, 48);
            this.lbLog.Name = "lbLog";
            this.lbLog.Size = new System.Drawing.Size(663, 394);
            this.lbLog.TabIndex = 0;
            // 
            // btDiscover
            // 
            this.btDiscover.Location = new System.Drawing.Point(12, 12);
            this.btDiscover.Name = "btDiscover";
            this.btDiscover.Size = new System.Drawing.Size(75, 23);
            this.btDiscover.TabIndex = 1;
            this.btDiscover.Text = "Discover";
            this.btDiscover.UseVisualStyleBackColor = true;
            this.btDiscover.Click += new System.EventHandler(this.btDiscover_Click);
            // 
            // cbUnpair
            // 
            this.cbUnpair.AutoSize = true;
            this.cbUnpair.Location = new System.Drawing.Point(105, 16);
            this.cbUnpair.Name = "cbUnpair";
            this.cbUnpair.Size = new System.Drawing.Size(132, 17);
            this.cbUnpair.TabIndex = 2;
            this.cbUnpair.Text = "Unpair before connect";
            this.cbUnpair.UseVisualStyleBackColor = true;
            // 
            // btClear
            // 
            this.btClear.Location = new System.Drawing.Point(600, 12);
            this.btClear.Name = "btClear";
            this.btClear.Size = new System.Drawing.Size(75, 23);
            this.btClear.TabIndex = 3;
            this.btClear.Text = "Clear";
            this.btClear.UseVisualStyleBackColor = true;
            this.btClear.Click += new System.EventHandler(this.btClear_Click);
            // 
            // fmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(687, 450);
            this.Controls.Add(this.btClear);
            this.Controls.Add(this.cbUnpair);
            this.Controls.Add(this.btDiscover);
            this.Controls.Add(this.lbLog);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "fmMain";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "RFCOMM Client Connection Test Application";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.Form1_FormClosed);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListBox lbLog;
        private System.Windows.Forms.Button btDiscover;
        private System.Windows.Forms.CheckBox cbUnpair;
        private System.Windows.Forms.Button btClear;
    }
}

