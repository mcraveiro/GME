using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace CSGUI
{
    internal class WindowWrapper : IWin32Window
    {
        public IntPtr Handle { get; private set; }
        public WindowWrapper(IntPtr hwnd) { Handle = hwnd; }
    }

    [Guid("68FDA295-8D49-4445-BE29-1F171B4BE07D"),
    ProgId("CSGUI.WelcomeScreenExp"),
    ClassInterface(ClassInterfaceType.AutoDual)]
    [ComVisible(true)]
    public class WelcomeScreenExp
    {
        public static string sCREATE_SENTINEL = "<create>";
        public string CREATE_SENTINEL { get { return sCREATE_SENTINEL; } }
        public string ShowWelcomeWindow(Int64 parentHwnd)
        {
            WelcomeScreen ws = new WelcomeScreen();
            ws.ShowDialog(new WindowWrapper((IntPtr)parentHwnd), recents);
            return ws.SelectedProject;
        }

        List<string> recents = new List<string>();

        public void SetRecentProjects(object recents)
        {
            foreach (string recent in (recents as Array))
            {
                if (!String.IsNullOrEmpty(recent))
                    this.recents.Add(recent);
            }
        }

        public void SwitchRef()
        {
            new ReferenceSwitcher().SwitchReference(null, null);
        }
    }

}
