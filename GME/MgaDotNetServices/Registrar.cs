using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Security.Principal;
using System.Runtime.InteropServices;
using System.Reflection;

namespace MGA.DotNetServices
{
    [Guid("0BB0C371-6835-4F09-A156-0BD8E3DF8216"),
    ProgId("MGA.DotNetRegistrar"),
    ClassInterface(ClassInterfaceType.AutoDual)]
    [ComVisible(true)]
    public class Registrar
    {
        public void Register(String filename)
        {
            // bool isElevated = new WindowsPrincipal(WindowsIdentity.GetCurrent()).IsInRole(WindowsBuiltInRole.Administrator);
            RegistrationServices regasm = new RegistrationServices();
            Assembly asm = Assembly.LoadFrom(filename);
            try
            {
                regasm.RegisterAssembly(asm, AssemblyRegistrationFlags.SetCodeBase);
            }
            catch (System.Reflection.TargetInvocationException e)
            {
                throw e.GetBaseException();
            }
        }

        public void Unregister(String filename)
        {
            RegistrationServices regasm = new RegistrationServices();
            Assembly asm = Assembly.LoadFrom(filename);
            try
            {
                regasm.UnregisterAssembly(asm);
            }
            catch (System.Reflection.TargetInvocationException e)
            {
                throw e.GetBaseException();
            }
        }
    }
}
