namespace CSGUI
{
    partial class WelcomeScreen
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
            this.btnNew = new System.Windows.Forms.Button();
            this.btnOpen = new System.Windows.Forms.Button();
            this.grpRecents = new System.Windows.Forms.GroupBox();
            this.SuspendLayout();
            // 
            // btnNew
            // 
            this.btnNew.BackgroundImage = global::CSGUI.Properties.Resources.New;
            this.btnNew.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.btnNew.Location = new System.Drawing.Point(12, 168);
            this.btnNew.Name = "btnNew";
            this.btnNew.Size = new System.Drawing.Size(152, 150);
            this.btnNew.TabIndex = 1;
            this.btnNew.Text = "New ";
            this.btnNew.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.btnNew.UseVisualStyleBackColor = true;
            // 
            // btnOpen
            // 
            this.btnOpen.BackgroundImage = global::CSGUI.Properties.Resources.Open;
            this.btnOpen.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.btnOpen.Location = new System.Drawing.Point(14, 12);
            this.btnOpen.Name = "btnOpen";
            this.btnOpen.Size = new System.Drawing.Size(150, 150);
            this.btnOpen.TabIndex = 0;
            this.btnOpen.Text = "Open ";
            this.btnOpen.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.btnOpen.UseVisualStyleBackColor = true;
            // 
            // grpRecents
            // 
            this.grpRecents.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.grpRecents.Location = new System.Drawing.Point(170, 12);
            this.grpRecents.Name = "grpRecents";
            this.grpRecents.Size = new System.Drawing.Size(919, 306);
            this.grpRecents.TabIndex = 2;
            this.grpRecents.TabStop = false;
            this.grpRecents.Text = "Recent Projects";
            // 
            // WelcomeScreen
            // 
            this.AllowDrop = true;
            this.AutoScaleDimensions = new System.Drawing.SizeF(96F, 96F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi;
            this.ClientSize = new System.Drawing.Size(872, 328);
            this.Controls.Add(this.grpRecents);
            this.Controls.Add(this.btnNew);
            this.Controls.Add(this.btnOpen);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Icon = global::CSGUI.Properties.Resources.GME;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "WelcomeScreen";
            this.Text = "GME";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button btnOpen;
        private System.Windows.Forms.Button btnNew;
        private System.Windows.Forms.GroupBox grpRecents;


    }
}