using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace CSGUI
{

    [ComVisible(false)]
    public partial class WelcomeScreen : Form
    {

        public string SelectedProject;
        public WelcomeScreen()
        {
            InitializeComponent();
            this.FormBorderStyle = FormBorderStyle.FixedSingle;
        }
        protected override bool ProcessMnemonic(char charCode)
        {
            if (charCode >= '1' && charCode < '1' + recents.Count)
            {
                this.SelectedProject = recents[charCode - '1'];
                Close();
                return true;
            }
            if (charCode == 'o')
            {
                this.btnOpen.PerformClick();
                return true;
            }
            if (charCode == 'n')
            {
                this.btnNew.PerformClick();
                return true;
            }
            return false;
        }

        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            if (keyData == Keys.Escape)
                this.Close();
            return base.ProcessCmdKey(ref msg, keyData);
        }

        List<string> recents;
        internal void ShowDialog(IWin32Window windowWrapper, List<string> recents)
        {
            this.recents = recents;
            int y = 20;
            using (Graphics g = this.CreateGraphics())
            {

            foreach (string recent_ in recents)
            {
                string recent = recent_;
                LinkLabel recentLink = new System.Windows.Forms.LinkLabel();
                recentLink.Text = recent.Substring("MGA=".Length);
                recentLink.Location = new Point(7, y);
                recentLink.AutoSize = false;
                recentLink.AutoEllipsis = true;
                recentLink.Font = new System.Drawing.Font("Segoe UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
                recentLink.Size = new Size(grpRecents.Size.Width - 14,
                    (int)Math.Ceiling(20 * g.DpiY / 96));
                recentLink.LinkClicked += delegate(object sender, LinkLabelLinkClickedEventArgs args)
                {
                    this.SelectedProject = recent;
                    this.Close();
                };
                grpRecents.Controls.Add(recentLink);
                y += 4 + recentLink.Size.Height;
            }
            }

            this.btnOpen.Click += new EventHandler(delegate(object o, EventArgs args)
            {
                OpenFileDialog openFileDialog1 = new OpenFileDialog();
                //openFileDialog1.InitialDirectory =
                openFileDialog1.Filter = "GME project files (*.mga,*.xme,*.mgx)|*.mga; *.xme; *.mgx|MGA files|*.mga|GME XML files|*.xme; *.xml|All files (*.*)|*.*";
                openFileDialog1.RestoreDirectory = true;
                openFileDialog1.CheckFileExists = true;

                if (openFileDialog1.ShowDialog(this) == DialogResult.OK)
                {
                    this.SelectedProject = openFileDialog1.FileName;
                    this.Close();
                }
            });
            this.btnNew.Click += new EventHandler(delegate(object o, EventArgs args)
                {
                    this.SelectedProject = WelcomeScreenExp.sCREATE_SENTINEL;
                    this.Close();
                });
            this.StartPosition = FormStartPosition.CenterParent;
            this.ShowInTaskbar = false;
            this.DragDrop += new DragEventHandler(delegate(object o, DragEventArgs de)
                {
                    if (de.Data.GetDataPresent(DataFormats.FileDrop))
                    {
                        SelectedProject = (de.Data.GetData(DataFormats.FileDrop) as string[])[0];
                        this.Close();
                    }
                });
            this.DragEnter += new DragEventHandler(delegate(object o, DragEventArgs e)
                {
                    if (e.Data.GetDataPresent(DataFormats.FileDrop))
                    {
                        string filename = (e.Data.GetData(DataFormats.FileDrop) as string[])[0];
                        if (filename.EndsWith(".mga") || filename.EndsWith(".xme"))
                        {
                            e.Effect = DragDropEffects.Copy;
                            return;
                        }
                    }
                    e.Effect = DragDropEffects.None;

                });
            ShowDialog(windowWrapper);
        }
    }
}
