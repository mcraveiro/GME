using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;
using System.Runtime.InteropServices;
using System.IO;
using GME.CSharp;
using GME.MGA;
using GME;
using GME.MGA.Core;

namespace MyAddon
{
    [Guid(ComponentConfig.guid),
    ProgId(ComponentConfig.progID),
    ClassInterface(ClassInterfaceType.AutoDual)]
    [ComVisible(true)]
    public class MyAddon : IMgaComponentEx, IGMEVersionInfo, IMgaEventSink
    {

        private MgaAddOn addon;
        private bool componentEnabled = true;
        private bool handleEvents = true;
        GMEConsole GMEConsole { get; set; }

        // Event handlers for addons
        #region MgaEventSink members
        public void GlobalEvent(globalevent_enum @event)
        {
            if (@event == globalevent_enum.GLOBALEVENT_CLOSE_PROJECT)
            {
                if (GMEConsole != null)
                {
                    if (GMEConsole.gme != null)
                    {
                        Marshal.FinalReleaseComObject(GMEConsole.gme);
                    }
                    GMEConsole = null;
                }
                Marshal.FinalReleaseComObject(addon);
                addon = null;
            }
            if (@event == globalevent_enum.APPEVENT_XML_IMPORT_BEGIN)
            {
                handleEvents = false;
                addon.EventMask = 0;
            }
            else if (@event == globalevent_enum.APPEVENT_XML_IMPORT_END)
            {
                unchecked { addon.EventMask = (uint)ComponentConfig.eventMask; }
                handleEvents = true;
            }
            else if (@event == globalevent_enum.APPEVENT_LIB_ATTACH_BEGIN)
            {
                addon.EventMask = 0;
                handleEvents = false;
            }
            else if (@event == globalevent_enum.APPEVENT_LIB_ATTACH_END)
            {
                unchecked { addon.EventMask = (uint)ComponentConfig.eventMask; }
                handleEvents = true;
            }
            if (!componentEnabled)
            {
                return;
            }

            // TODO: Handle global events
            // MessageBox.Show(@event.ToString());
        }

        /// <summary>
        /// Called when an FCO or folder changes
        /// </summary>
        /// <param name="subject">the object the event(s) happened to</param>
        /// <param name="eventMask">objectevent_enum values ORed together</param>
        /// <param name="param">extra information provided for cetertain event types</param>
        public void ObjectEvent(MgaObject subject, uint eventMask, object param)
        {
            if (!componentEnabled || !handleEvents)
            {
                return;
            }
            if (GMEConsole == null)
            {
                GMEConsole = GMEConsole.CreateFromProject(subject.Project);
            }

            // TODO: Handle object events (OR eventMask with the members of objectevent_enum)
            // Warning: Only those events are received that you have subscribed for by setting ComponentConfig.eventMask

            // If the event is OBJEVENT_DESTROYED, most operations on subject will fail
            //   Safe operations: getting Project, ObjType, ID, MetaRole, Meta, MetaBase, Name, AbsPath
            //   Operations that will fail: all others, including attribute access and graph navigation
            //     Try handling OBJEVENT_PRE_DESTROYED if these operations are necessary

            // Be careful not to modify Library objects (check subject.IsLibObject)

            // MessageBox.Show(eventMask.ToString());
            // GMEConsole.Out.WriteLine(subject.Name);

        }

        #endregion

        #region IMgaComponentEx Members

        public void Initialize(MgaProject project)
        {
            // Creating addon
            project.CreateAddOn(this, out addon);
            // Setting event mask (see ComponentConfig.eventMask)
            unchecked
            {
                addon.EventMask = (uint)ComponentConfig.eventMask;
            }
        }

        public void InvokeEx(MgaProject project, MgaFCO currentobj, MgaFCOs selectedobjs, int param)
        {
            throw new NotImplementedException(); // Not called by addon
        }


        #region Component Information
        public string ComponentName
        {
            get { return GetType().Name; }
        }

        public string ComponentProgID
        {
            get
            {
                return ComponentConfig.progID;
            }
        }

        public componenttype_enum ComponentType
        {
            get { return ComponentConfig.componentType; }
        }
        public string Paradigm
        {
            get { return ComponentConfig.paradigmName; }
        }
        #endregion

        #region Enabling
        bool enabled = true;
        public void Enable(bool newval)
        {
            enabled = newval;
        }
        #endregion

        #region Interactive Mode
        protected bool interactiveMode = true;
        public bool InteractiveMode
        {
            get
            {
                return interactiveMode;
            }
            set
            {
                interactiveMode = value;
            }
        }
        #endregion

        #region Custom Parameters
        SortedDictionary<string, object> componentParameters = null;

        public object get_ComponentParameter(string Name)
        {
            if (Name == "type")
                return "csharp";

            if (Name == "path")
                return GetType().Assembly.Location;

            if (Name == "fullname")
                return GetType().FullName;

            object value;
            if (componentParameters != null && componentParameters.TryGetValue(Name, out value))
            {
                return value;
            }

            return null;
        }

        public void set_ComponentParameter(string Name, object pVal)
        {
            if (componentParameters == null)
            {
                componentParameters = new SortedDictionary<string, object>();
            }

            componentParameters[Name] = pVal;
        }
        #endregion

        #region Unused Methods
        // Old interface, it is never called for MgaComponentEx interfaces
        public void Invoke(MgaProject Project, MgaFCOs selectedobjs, int param)
        {
            throw new NotImplementedException();
        }

        // Not used by GME
        public void ObjectsInvokeEx(MgaProject Project, MgaObject currentobj, MgaObjects selectedobjs, int param)
        {
            throw new NotImplementedException();
        }

        #endregion

        #endregion

        #region IMgaVersionInfo Members

        public GMEInterfaceVersion_enum version
        {
            get { return GMEInterfaceVersion_enum.GMEInterfaceVersion_Current; }
        }

        #endregion

        #region Registration Helpers

        [ComRegisterFunctionAttribute]
        public static void GMERegister(Type t)
        {
            Registrar.RegisterComponentsInGMERegistry();
        }

        [ComUnregisterFunctionAttribute]
        public static void GMEUnRegister(Type t)
        {
            Registrar.UnregisterComponentsInGMERegistry();
        }

        #endregion
    }
}
