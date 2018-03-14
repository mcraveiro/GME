using System;
using System.Collections.Generic;
using System.Text;

namespace ##1##
{
 
    public interface IObject
    {
        string Name { get; set; }
        string ID { get; }
        MGALib.IMgaObject MgaObject{ get; }
        void Delete();
    }

    public interface IContainer
    { }

    public interface IFolder : IObject, IContainer
    { }

    public interface IFCO : IObject
    { }

    public interface IModel : IFCO, IContainer
    { }

    public interface IAtom : IFCO
    { }

    public interface IReference : IFCO
    { }

    public interface ISet : IFCO
    { }

    public interface IConnection : IFCO
    { }
}
