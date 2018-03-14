using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Drawing;

// This sample decorator draws the label and a black outline
// You must change the ProgId and Guid below
// (To generate a new Guid in Visual Studio, select Tools>Create Guid, Select format 5 and press Copy)

namespace GMEDecorator
{
    [Guid("7A5231C3-ED23-4E4F-906F-E2EE67D5B62F"),
    ProgId("MGA.SampleDecorator"),
    ClassInterface(ClassInterfaceType.AutoDual)]
    [ComVisible(true)]
    public class Decorator : GME.IMgaElementDecorator
    {
        #region IMgaElementDecorator Members

        public void Destroy()
        {
            this.MgaObject = null;
            this.Project = null;
            this.MetaPart = null;
        }

        public void DragEnter(out uint dropEffect, ulong pCOleDataObject, uint keyState, int pointx, int pointy, ulong transformHDC)
        {
            dropEffect = 0;
        }

        public void DragOver(out uint dropEffect, ulong pCOleDataObject, uint keyState, int pointx, int pointy, ulong transformHDC)
        {
            dropEffect = 0;
        }

        public void Draw(uint hdc)
        {
            Graphics g;
            unchecked { g = Graphics.FromHdc((IntPtr)(int)hdc); }
            using (g)
            {
                g.DrawRectangle(new Pen(Color.Black), Position);
                g.DrawString(Label, LabelFont, new SolidBrush(Color.Black), new PointF(LabelLocation.Left, LabelLocation.Top));
            }
        }

        public void DrawEx(uint hdc, ulong gdip)
        {
            Draw(hdc);
        }

        public void Drop(ulong pCOleDataObject, uint dropEffect, int pointx, int pointy, ulong transformHDC)
        {
        }

        public void DropFile(ulong hDropInfo, int pointx, int pointy, ulong transformHDC)
        {
        }

        public void GetFeatures(out uint features)
        {
            features = 0;
        }

        public string Label
        {
            get;
            set;
        }

        public Font LabelFont
        {
            get { return new Font("Arial", 12); }
        }

        public Rectangle LabelLocation { get; set; }
        public void GetLabelLocation(out int sx, out int sy, out int ex, out int ey)
        {
            sx = LabelLocation.Left;
            sy = LabelLocation.Top;
            ex = LabelLocation.Width;
            ey = LabelLocation.Height;
        }

        public void GetLocation(out int sx, out int sy, out int ex, out int ey)
        {
            sx = Position.Left;
            sy = Position.Top;
            ex = Position.Width;
            ey = Position.Height;
        }

        public void GetMnemonic(out string mnemonic)
        {
            throw new NotImplementedException();
        }

        public void GetParam(string Name, out object value)
        {
            throw new NotImplementedException();
        }

        public void GetPortLocation(GME.MGA.MgaFCO fco, out int sx, out int sy, out int ex, out int ey)
        {
            throw new NotImplementedException();
        }

        public GME.MGA.MgaFCOs GetPorts()
        {
            return (GME.MGA.MgaFCOs)Activator.CreateInstance(Type.GetTypeFromProgID("MGA.MgaFCOs"));
        }

        public void GetPreferredSize(out int sizex, out int sizey)
        {
            // TODO
            sizex = 150;
            sizey = 150;
        }

        public void Initialize(GME.MGA.MgaProject p, GME.MGA.Meta.MgaMetaPart meta, GME.MGA.MgaFCO obj)
        {
            InitializeEx(p, meta, obj, null, 0);
        }

        //GME.MGA.MgaProject 
        public GME.MGA.MgaProject Project { get; set; }
        public GME.MGA.Meta.MgaMetaPart MetaPart { get; set; }
        // Object is null when the decorator is drawing an element in the Part Browser
        public GME.MGA.MgaFCO MgaObject { get; set; }
        IntPtr parentHwnd;
        SizeF LabelSize;
        public void InitializeEx(GME.MGA.MgaProject p, GME.MGA.Meta.MgaMetaPart meta, GME.MGA.MgaFCO obj, GME.IMgaCommonDecoratorEvents eventSink, ulong parentWnd)
        {
            Project = p;
            MetaPart = meta;
            MgaObject = obj;
            unchecked { parentHwnd = (IntPtr)(int)parentWnd; }

            if (MgaObject != null)
                Label = MgaObject.Name;
            else
                Label = MetaPart.DisplayedName;
            using (Graphics g = Graphics.FromHwnd(parentHwnd))
            {
                LabelSize = g.MeasureString(Label, LabelFont);
            }
        }

        public void MenuItemSelected(uint menuItemId, uint nFlags, int pointx, int pointy, ulong transformHDC)
        {
        }

        public void MouseLeftButtonDoubleClick(uint nFlags, int pointx, int pointy, ulong transformHDC)
        {
        }

        public void MouseLeftButtonDown(uint nFlags, int pointx, int pointy, ulong transformHDC)
        {
        }

        public void MouseLeftButtonUp(uint nFlags, int pointx, int pointy, ulong transformHDC)
        {
        }

        public void MouseMiddleButtonDoubleClick(uint nFlags, int pointx, int pointy, ulong transformHDC)
        {
        }

        public void MouseMiddleButtonDown(uint nFlags, int pointx, int pointy, ulong transformHDC)
        {
        }

        public void MouseMiddleButtonUp(uint nFlags, int pointx, int pointy, ulong transformHDC)
        {
        }

        public void MouseMoved(uint nFlags, int pointx, int pointy, ulong transformHDC)
        {
        }

        public void MouseRightButtonDoubleClick(uint nFlags, int pointx, int pointy, ulong transformHDC)
        {
        }

        public void MouseRightButtonDown(ulong hCtxMenu, uint nFlags, int pointx, int pointy, ulong transformHDC)
        {
        }

        public void MouseRightButtonUp(uint nFlags, int pointx, int pointy, ulong transformHDC)
        {
        }

        public void MouseWheelTurned(uint nFlags, int distance, int pointx, int pointy, ulong transformHDC)
        {
        }

        public void OperationCanceled()
        {
        }

        public void SaveState()
        {
        }

        public void SetActive(bool isActive)
        {
        }

        public System.Drawing.Rectangle Position { get; set; }
        public void SetLocation(int sx, int sy, int ex, int ey)
        {
            Position = new System.Drawing.Rectangle(sx, sy, ex - sx - 1, ey - sy - 1);
            LabelLocation = new Rectangle(Position.Left + (Position.Width / 2) - ((int)LabelSize.Width / 2),
                Position.Bottom + 5,
                (int)LabelSize.Width,
                (int)LabelSize.Height + 5);
        }

        public void SetParam(string Name, object value)
        {
        }

        public bool Selected { get; set; }
        public void SetSelected(bool isSelected)
        {
            Selected = isSelected;
        }

        #endregion
    }
}
