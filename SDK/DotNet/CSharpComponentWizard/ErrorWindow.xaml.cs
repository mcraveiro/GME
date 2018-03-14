using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace CSharpComponentWizard
{
    
    public partial class ErrorWindow : Window
    {
        private MainWindow parent;
        
        public ErrorWindow(MainWindow mw)
        {
            InitializeComponent();
            this.parent = mw;
        }

        private void btn_errorexit_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
            this.parent.Close();
        }

        private void btn_errorignore_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        public void Display(List<string> errors)
        {
            this.ErrorList.Items.Clear();
            foreach (string err in errors)
            {
                this.ErrorList.Items.Add(err);
            }
            this.ShowDialog();
        }
    }
}
