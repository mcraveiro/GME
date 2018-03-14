namespace CSharpDSMLGenerator
{
	partial class ApiGenerator
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ApiGenerator));
            this.cbMode = new System.Windows.Forms.ComboBox();
            this.txtOutputDir = new System.Windows.Forms.TextBox();
            this.btnOK = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // cbMode
            // 
            this.cbMode.DisplayMember = "1";
            this.cbMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbMode.FormattingEnabled = true;
            this.cbMode.Location = new System.Drawing.Point(12, 73);
            this.cbMode.Name = "cbMode";
            this.cbMode.Size = new System.Drawing.Size(416, 21);
            this.cbMode.TabIndex = 0;
            // 
            // txtOutputDir
            // 
            this.txtOutputDir.Enabled = false;
            this.txtOutputDir.Location = new System.Drawing.Point(12, 47);
            this.txtOutputDir.Name = "txtOutputDir";
            this.txtOutputDir.Size = new System.Drawing.Size(416, 20);
            this.txtOutputDir.TabIndex = 1;
            this.txtOutputDir.Text = "Output directory";
            // 
            // btnOK
            // 
            this.btnOK.Location = new System.Drawing.Point(183, 164);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 1;
            this.btnOK.Text = "OK";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // ApiGenerator
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(439, 193);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.txtOutputDir);
            this.Controls.Add(this.cbMode);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "ApiGenerator";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "ApiGenerator";
            this.ResumeLayout(false);
            this.PerformLayout();

		}

		#endregion

		internal System.Windows.Forms.ComboBox cbMode;
		internal System.Windows.Forms.TextBox txtOutputDir;
		private System.Windows.Forms.Button btnOK;
	}
}