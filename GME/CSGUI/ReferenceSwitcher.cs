using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using GME.MGA.Core;
using GME.CSharp;
using GME;
using GME.MGA;
using System.Linq;

namespace CSGUI
{
    /// <summary>
    /// This class implements the necessary COM interfaces for a GME interpreter component.
    /// </summary>
    [Guid("0ACC000C-29E6-418B-9F8B-968418C533B9"),
    ProgId("CSGUI.ReferenceSwitcher"),
    ClassInterface(ClassInterfaceType.AutoDual)]
    [ComVisible(true)]
    public class ReferenceSwitcher
    {
        MgaGateway MgaGateway { get; set; }
        GMEConsole GMEConsole { get; set; }

        public void SwitchReferences(object[] lib1Objects, object[] lib2Objects)
        {
            Switcher switcher = new Switcher(lib1Objects.Cast<IMgaObject>(), lib2Objects.Cast<IMgaObject>(), null);
            switcher.UpdateSublibrary();
        }

        [ComVisible(false)]
        public void PrintLine(Func<string, string, string> f, IMgaFCO a, IMgaFCO b)
        {
            Console.Out.WriteLine(f(Switcher.getPath(a), Switcher.getPath(b)));
        }
        public void SwitchReference(IMgaFCO to, IMgaReference @ref)
        {
            Switcher.MoveReferenceWithRefportConnections(to, @ref, PrintLine);
        }
    }

    [ComVisible(false)]
    public class Switcher
    {
        List<IMgaObject> lib1Objects;
        List<IMgaObject> lib2Objects;
        GMEConsole GMEConsole;

        public Switcher(IMgaObject fco1Object, IMgaObject fco2Object, GMEConsole GMEConsole)
        {
            this.lib1Objects = new List<IMgaObject>() { fco1Object };
            this.lib2Objects = new List<IMgaObject>() { fco2Object };
            this.GMEConsole = GMEConsole;
        }
        public Switcher(IEnumerable<IMgaObject> fco1Object, IEnumerable<IMgaObject> fco2Object, GMEConsole GMEConsole)
        {
            this.lib1Objects = fco1Object.ToList();
            this.lib2Objects = fco2Object.ToList();
            this.GMEConsole = GMEConsole;
        }

        class ObjectPair
        {
            public IMgaObject o1 { get; set; }
            public IMgaObject o2 { get; set; }
        }

        public void UpdateSublibrary()
        {
            int origPrefs = this.lib1Objects.First().Project.Preferences;
            // Magic word allows us to remove ConnPoints
            this.lib1Objects.First().Project.Preferences = origPrefs | (int)GME.MGA.preference_flags.MGAPREF_IGNORECONNCHECKS
                | (int)GME.MGA.preference_flags.MGAPREF_FREEINSTANCEREFS;
            try
            {
                IEnumerator<IMgaObject> lib2ObjectsEnum = lib2Objects.GetEnumerator();
                foreach (IMgaObject lib1Object in lib1Objects)
                {
                    lib2ObjectsEnum.MoveNext();
                    UpdateSublibrary(lib1Object, lib2ObjectsEnum.Current);
                }
            }
            finally
            {
                lib1Objects.First().Project.Preferences = origPrefs;
            }
        }

        private void UpdateSublibrary(IMgaObject fco1Objec, IMgaObject fco2Objec)
        {
            // fco2Object may be null
            if (fco1Objec is IMgaFCO) // references only refer to FCOs
            {
                IMgaFCO fco1 = (IMgaFCO)fco1Objec;
                IMgaFCO fco2 = fco2Objec as IMgaFCO;
                foreach (IMgaFCO fco in fco1.ReferencedBy)
                {
                    if (fco.IsInstance)
                        continue; // instance references will be updated by their archetype
                    // Don't update references in the old library
                    bool fcoInLib1Objects = false;
                    foreach (IMgaObject lib1Root in this.lib1Objects)
                    {
                        // FIXME: Contains(this.fco1Object) doesn't work
                        if (new ParentChain(fco).Contains(lib1Root, new MgaObjectEqualityComparor<IMgaObject>()))
                        {
                            fcoInLib1Objects = true;
                        }
                    }
                    if (fcoInLib1Objects)
                        continue;
                    IMgaReference refe = (IMgaReference)fco;
                    if (fco2 != null)
                    {
                        if (refe.UsedByConns.Count != 0)
                        {
                            if (refe.DerivedFrom == null)
                            {
                                try
                                {
                                    MoveReferenceWithRefportConnections(fco2, refe, WriteLine);
                                }
                                catch (Exception e)
                                {
                                    if (GMEConsole != null)
                                        GMEConsole.Error.WriteLine("Could not set reference " + GetLink(refe, refe.Name));
                                    throw new Exception("Could not set reference " + getPath(refe) +
                                        " (" + refe.ID + ")", e);
                                }
                            }
                        }
                        else
                        {
                            try
                            {
                                bool setRef;
                                if (refe.DerivedFrom == null)
                                    setRef = true;
                                else
                                {
                                    short compareToBase;
                                    refe.CompareToBase(out compareToBase);
                                    setRef = compareToBase != 0;
                                }
                                if (setRef)
                                {
                                    // FIXME: can this fail; should we handle it somehow?
                                    refe.Referred = (GME.MGA.MgaFCO)fco2;
                                }
                            }
                            catch (Exception e)
                            {
                                if (GMEConsole != null)
                                    GMEConsole.Error.WriteLine("Could not set reference " + GetLink(refe, refe.Name));
                                throw new Exception("Could not set reference " + getPath(refe) +
                                    " (" + refe.ID + ")", e);
                            }
                        }
                    }
                    else
                    {
                        WriteLine((x, y) => "Couldn't update " + x + ": " + y + " has no counterpart", refe, fco1);
                    }
                }
            }

            List<IMgaObject> fco1Children = getChildren(fco1Objec);
            List<IMgaObject> fco2Children = getChildren(fco2Objec);
            Dictionary<string, ObjectPair> dict = new Dictionary<string, ObjectPair>();
            foreach (IMgaObject o in fco1Children)
            {
                dict.GetValueOrDefault(o.Name + "xxx ;xxx" + o.MetaBase.Name).o1 = o;
            }
            foreach (IMgaObject o in fco2Children)
            {
                dict.GetValueOrDefault(o.Name + "xxx ;xxx" + o.MetaBase.Name).o2 = o;
            }
            foreach (KeyValuePair<string, ObjectPair> entry in dict)
            {
                if (entry.Value.o1 != null)
                {
                    UpdateSublibrary(entry.Value.o1, entry.Value.o2);
                }
            }
        }


        public delegate void WriteLineF(Func<string, string, string> f, IMgaFCO a, IMgaFCO b);
        public void WriteLine(Func<string, string, string> f, IMgaFCO a, IMgaFCO b)
        {
            if (GMEConsole != null)
            {
                GMEConsole.Out.WriteLine(f(GetLink(a, a.Name), GetLink(b, b.Name)));
            }
            else
            {
                Console.Out.WriteLine(f(getPath(a), getPath(b)));
            }
        }

        public static List<T> MakeList<T>(T itemOftype)
        {
            List<T> newList = new List<T>();
            return newList;
        }

        /**
         * First we disconnect all connections to refports, then move the reference, then reconnect
         */
        public static void MoveReferenceWithRefportConnections(IMgaFCO fco2, IMgaReference origref,
            WriteLineF WriteLine)
        {
            Queue<IMgaReference> references = new Queue<IMgaReference>();
            references.Enqueue(origref);
            IMgaFCO targetModel = fco2;
            while (targetModel is IMgaReference)
            {
                targetModel = ((IMgaReference)targetModel).Referred;
            }
            MgaFCOs fco2ChildFCOs = ((IMgaModel)targetModel).ChildFCOs;
            Dictionary<string, IMgaFCO> newRefeChildren = GetNameMap(fco2ChildFCOs,
                x => { });
            // TODO: warn, but only for refport-connected children
            //GMEConsole.Warning.WriteLine("Warning: " + fco2.Name + " has multiple children named " + x));

            int origPrefs = fco2.Project.Preferences;
            // Magic word allows us to remove ConnPoints
            fco2.Project.Preferences = origPrefs | (int)GME.MGA.preference_flags.MGAPREF_IGNORECONNCHECKS
                | (int)GME.MGA.preference_flags.MGAPREF_FREEINSTANCEREFS;

            try {

                MgaConnection conn = null;
                var ReconnectList = MakeList( new { ConnRole = "src", Ref = origref, Port = fco2, Conn = conn } );
                while( references.Count != 0 ) {
                    IMgaReference refe = references.Dequeue();

                    foreach (IMgaConnPoint connPoint in refe.UsedByConns) {
                        if (connPoint.References[1] != refe)
                        {
                            continue;
                        }
                        IMgaFCO fco2Port;
                        if( newRefeChildren.TryGetValue( connPoint.Target.Name, out fco2Port ) ) {
                            if( fco2Port == null ) {
                                // fco2Port == null => multiple children with the same name
                                // Try matching based on Kind too
                                fco2Port = fco2ChildFCOs.Cast<IMgaFCO>().Where( x => x.Name == connPoint.Target.Name
                                    && x.Meta.MetaRef == connPoint.Target.Meta.MetaRef ).FirstOrDefault();
                            }
                            if( fco2Port != null ) {
                                ReconnectList.Add( new { ConnRole = connPoint.ConnRole, Ref = refe, Port = fco2Port, Conn = connPoint.Owner } );
                                connPoint.Remove();
                            }
                        } else {
                            WriteLine( ( x, y ) => "Can't find corresponding port for " + x
                                + " in " + y, connPoint.Target, fco2 );
                            connPoint.Owner.DestroyObject();
                        }
                    }
                    foreach( IMgaReference x in refe.ReferencedBy.Cast<IMgaReference>() ) {
                        if( x.ID == origref.ID )
                            throw new Exception( "Circular reference chain starting with " + origref.AbsPath );
                        references.Enqueue( x );
                    }
                }
                origref.Referred = (MgaFCO)fco2;
                foreach( var reconnect in ReconnectList ) {
                    if( reconnect.ConnRole == "src" )
                        ( reconnect.Conn as IMgaSimpleConnection ).SetSrc( (MgaFCOs)GetRefChain( reconnect.Ref ), (MgaFCO)reconnect.Port );
                    else
                        ( reconnect.Conn as IMgaSimpleConnection ).SetDst( (MgaFCOs)GetRefChain( reconnect.Ref ), (MgaFCO)reconnect.Port );
                }

            } finally {
                fco2.Project.Preferences = origPrefs;
            }

        }

        public static string GetLink(IMgaObject o, string linkText = null)
        {
            if (linkText == null)
            {
                linkText = getPath(o);
            }
            return "<a href=\"mga:" + o.ID + "\">"
                + linkText
                + "</a>";
        }

        private static Dictionary<string, IMgaFCO> GetNameMap(IMgaFCOs fcos,
            Action<string> warnFunc)
        {
            Dictionary<string, IMgaFCO> refeChildren;
            refeChildren = new Dictionary<string, IMgaFCO>();
            foreach (IMgaFCO refeChild in fcos.Cast<IMgaFCO>())
            {
                if (refeChild.ObjType == GME.MGA.Meta.objtype_enum.OBJTYPE_CONNECTION)
                    continue;
                if (refeChildren.ContainsKey(refeChild.Name))
                {
                    warnFunc(refeChild.Name);
                    refeChildren[refeChild.Name] = null;
                }
                else
                    refeChildren.Add(refeChild.Name, refeChild);
            }
            return refeChildren;
        }

        private static IMgaFCOs GetRefChain(IMgaReference reference)
        {
            IMgaFCOs ret = (IMgaFCOs)Activator.CreateInstance(Type.GetTypeFromProgID("Mga.MgaFCOs"));
            ret.Append(reference as MgaFCO);
            while (true)
            {
                if (reference.Referred == null)
                {
                    break;
                }
                if (reference.Referred.ObjType == GME.MGA.Meta.objtype_enum.OBJTYPE_REFERENCE)
                {
                    reference = reference.Referred as IMgaReference;
                    ret.Append(reference as MgaFCO);
                }
                else
                {
                    break;
                }
            }
            return ret;
        }

        public static string getPath(IMgaObject fco1Object)
        {
            return String.Join("/", new ParentChain(fco1Object).Select(x => x.Name).Reverse().ToArray());
        }

        private List<IMgaObject> getChildren(IMgaObject fco1Objec)
        {
            List<IMgaObject> fco1Children = new List<IMgaObject>();
            if (fco1Objec == null)
            {
                return fco1Children;
            }
            if (fco1Objec is IMgaFolder)
            {
                fco1Children.AddRange((fco1Objec as IMgaFolder).ChildFolders.Cast<IMgaObject>());
                fco1Children.AddRange((fco1Objec as IMgaFolder).ChildFCOs.Cast<IMgaObject>());
            }
            if (fco1Objec is IMgaModel)
            {
                fco1Children.AddRange((fco1Objec as IMgaModel).ChildFCOs.Cast<IMgaObject>());
            }
            return fco1Children;
        }
    }

    [ComVisible(false)]
    class MgaObjectEqualityComparor<T> : EqualityComparer<T> where T : IMgaObject
    {
        public override bool Equals(T x, T y)
        {
            return x.ID.Equals(y.ID);
        }

        public override int GetHashCode(T obj)
        {
            return obj.ID.GetHashCode();
        }
    }

    [ComVisible(false)]
    static class DictionaryExtension
    {
        public static V GetValueOrDefault<K, V>(this Dictionary<K, V> dic, K key)
            where V : new()
        {
            V ret;
            bool found = dic.TryGetValue(key, out ret);
            if (found)
            {
                return ret;
            }
            else
            {
                ret = new V();
                dic[key] = ret;
                return ret;
            }
        }
    }
}
