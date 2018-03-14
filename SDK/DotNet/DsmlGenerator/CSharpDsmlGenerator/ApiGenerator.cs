using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace CSharpDSMLGenerator
{
	public partial class ApiGenerator : Form
	{
		public ApiGenerator()
		{
			InitializeComponent();


			cbMode.Items.Add(new ModeItem()
			{
				Mode = CSharpDSMLGeneratorInterpreter.GeneratorMode.OneFile,
				Text = "Generate one file",
			});

			cbMode.Items.Add(new ModeItem()
			{
				Mode = CSharpDSMLGeneratorInterpreter.GeneratorMode.Namespaces,
				Text = "Generate two files (classes and interfaces)",
			});

			cbMode.Items.Add(new ModeItem()
			{
				Mode = CSharpDSMLGeneratorInterpreter.GeneratorMode.Many,
				Text = "Generate separate files (for each class and interface)",
			});


			// namespaces by default
			cbMode.SelectedItem = cbMode.Items.
				Cast<ModeItem>().
				FirstOrDefault(x => x.Mode == CSharpDSMLGeneratorInterpreter.GeneratorMode.Namespaces);

			btnOK.Select();
		}

		private void btnOK_Click(object sender, EventArgs e)
		{
			if (cbMode.SelectedItem == null)
			{
				MessageBox.Show("Selected mode null.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
				return;
			}
			DialogResult = System.Windows.Forms.DialogResult.OK;
			Close();
		}

		public class ModeItem
		{
			public CSharpDSMLGeneratorInterpreter.GeneratorMode Mode { get; set; }

			public string Text { get; set; }

			public override string ToString()
			{
				return String.Format("[{0}] {1}", Mode, Text);
			}
		}
	}
}
