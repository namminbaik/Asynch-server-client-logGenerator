namespace SimpleCSharpAsyncServer
{
    partial class Form1
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
            this.logConsole = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.logDeletion = new System.Windows.Forms.CheckBox();
            this.backupStorage = new System.Windows.Forms.CheckBox();
            this.button1 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // logConsole
            // 
            this.logConsole.BackColor = System.Drawing.Color.Black;
            this.logConsole.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.logConsole.ForeColor = System.Drawing.Color.White;
            this.logConsole.Location = new System.Drawing.Point(10, 9);
            this.logConsole.Multiline = true;
            this.logConsole.Name = "logConsole";
            this.logConsole.ReadOnly = true;
            this.logConsole.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.logConsole.Size = new System.Drawing.Size(650, 431);
            this.logConsole.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(666, 19);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(53, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Detect All";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(666, 46);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(84, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Backup Storage";
            // 
            // logDeletion
            // 
            this.logDeletion.AutoSize = true;
            this.logDeletion.Location = new System.Drawing.Point(764, 19);
            this.logDeletion.Name = "logDeletion";
            this.logDeletion.Size = new System.Drawing.Size(15, 14);
            this.logDeletion.TabIndex = 3;
            this.logDeletion.UseVisualStyleBackColor = true;
            // 
            // backupStorage
            // 
            this.backupStorage.AutoSize = true;
            this.backupStorage.Location = new System.Drawing.Point(764, 46);
            this.backupStorage.Name = "backupStorage";
            this.backupStorage.Size = new System.Drawing.Size(15, 14);
            this.backupStorage.TabIndex = 4;
            this.backupStorage.UseVisualStyleBackColor = true;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(669, 80);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(110, 24);
            this.button1.TabIndex = 5;
            this.button1.Text = "Send Policy";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.backupStorage);
            this.Controls.Add(this.logDeletion);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.logConsole);
            this.Name = "Form1";
            this.Text = "SLCP Server";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox logConsole;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.CheckBox logDeletion;
        private System.Windows.Forms.CheckBox backupStorage;
        private System.Windows.Forms.Button button1;
    }
}

