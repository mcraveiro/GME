using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GME.MGA;
using System.Diagnostics.Contracts;
using GME.MGA.Meta;

namespace ISIS.GME.Common
{
    /// <summary>
    /// 
    /// </summary>
    public static class Utils
    {
        public static bool CheckMetaCompatibility(
            MgaProject project,
            ISIS.GME.Common.Interfaces.RootFolder rootFolder)
        {
            bool result = true;
            foreach (var item in rootFolder.MetaRefs)
            {
                try
                {
                    MgaMetaBase b = project.RootMeta.FindObject[item.Key];
                    if (b == null)
                    {
                        throw new Exception("Meta incompatibility");
                    }
                    else
                    {
                        // TODO: Namespaces??? this will not work
                        if (b.Name != item.Value.Name)
                        {
                            throw new Exception("Meta incompatibility");
                        }
                    }
                }
                catch (Exception ex)
                {
                    throw new Exception("Meta incompatibility", ex);
                }
            }

            return result;
        }


        public static T CreateObject<T>(
            ISIS.GME.Common.Interfaces.Container parent,
            string roleStr = null)
            where T : ISIS.GME.Common.Classes.Base, new()
        {
            Contract.Requires(parent != null);

            T result = new T();
            string Kind = typeof(T).Name;

            if (parent.Impl is IMgaModel)
            {
                IMgaModel model = parent.Impl as IMgaModel;
                MgaMetaRole role = null;

                if (string.IsNullOrEmpty(roleStr))
                {
                    try
                    {
                        // try to use user defined role
                        role = (model.MetaBase as IMgaMetaModel).RoleByName[Kind];
                    }
                    catch (Exception ex)
                    {
                        StringBuilder sb = new StringBuilder();
                        sb.AppendLine("Role was not found in the container.");
                        sb.AppendLine("Paradigm violation.");
                        sb.AppendFormat("Container type: {0} Requested role: {1}",
                            parent.Kind,
                            result.GetType().Name);
                        throw new Exception(sb.ToString(), ex);
                    }
                }
                else
                {
                    try
                    {
                        // try to use user defined role
                        role = (model.MetaBase as IMgaMetaModel).RoleByName[roleStr];
                    }
                    catch (Exception ex)
                    {
                        StringBuilder sb = new StringBuilder();
                        sb.AppendLine("Role was not found in the container.");
                        sb.AppendLine("Paradigm violation.");
                        sb.AppendFormat("Container type: {0} Requested role: {1}",
                            parent.Kind,
                            result.GetType().Name);
                        throw new Exception(sb.ToString(), ex);
                    }
                }
                try
                {
                    MgaFCO fco = model.CreateChildObject(role);
                    result.Impl = fco as IMgaObject;
                    return result;
                }
                catch (Exception ex)
                {
                    StringBuilder sb = new StringBuilder();
                    sb.AppendLine("New element could not be created.");
                    sb.AppendFormat("Container type: {0} Requested role: {1}",
                        parent.Kind,
                        result.GetType().Name);
                    throw new Exception(sb.ToString(), ex);
                }
            }
            else if (parent.Impl is MgaFolder)
            {
                if (string.IsNullOrEmpty(roleStr))
                {
                    MgaFolder folder = parent.Impl as MgaFolder;

                    foreach (MgaMetaFolder item in folder.MetaFolder.LegalChildFolders)
                    {
                        if (item.Name == Kind)
                        {
                            MgaFolder f = folder.CreateFolder(item);
                            result.Impl = f as IMgaObject;
                            return result;
                        }
                    }
                    if (result.Impl == null)
                    {
                        foreach (MgaMetaFCO item in folder.MetaFolder.LegalRootObjects)
                        {
                            if (item.Name == Kind)
                            {
                                IMgaFCO fco = folder.CreateRootObject(item);
                                result.Impl = fco as IMgaObject;
                                return result;
                            }
                        }
                    }
                }
                else
                {
                    MgaFolder folder = parent.Impl as MgaFolder;

                    foreach (MgaMetaFolder item in folder.MetaFolder.LegalChildFolders)
                    {
                        if (item.Name == roleStr)
                        {
                            MgaFolder f = folder.CreateFolder(item);
                            result.Impl = f as IMgaObject;
                            return result;
                        }
                    }
                    if (result.Impl == null)
                    {
                        foreach (MgaMetaFCO item in folder.MetaFolder.LegalRootObjects)
                        {
                            if (item.Name == roleStr)
                            {
                                IMgaFCO fco = folder.CreateRootObject(item);
                                result.Impl = fco as IMgaObject;
                                return result;
                            }
                        }
                    }
                }
            }

            return null;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="parent"></param>
        /// <param name="metaRef">meta ref of the new object</param>
        /// <param name="roleMetaRef">meta ref of the role (if the parent is a model)</param>
        /// <returns></returns>
        public static T CreateObject<T>(
            ISIS.GME.Common.Interfaces.Container parent,
            int metaRef,
            int roleMetaRef = 0)
            where T : ISIS.GME.Common.Classes.Base, new()
        {
            Contract.Requires(parent != null);

            T result = new T();

            if (parent.Impl is MgaModel)
            {
                MgaModel model = parent.Impl as MgaModel;
                MgaMetaRole role = null;
                try
                {
                    // try to use user defined role
                    role = (model.MetaBase as MgaMetaModel).
                        Roles.
                        Cast<MgaMetaRole>().
                        FirstOrDefault(x => x.MetaRef == roleMetaRef);
                }
                catch (Exception ex)
                {
                    StringBuilder sb = new StringBuilder();
                    sb.AppendLine("Role was not found in the container.");
                    sb.AppendLine("Paradigm violation.");
                    sb.AppendFormat("Container type: {0} Requested role: {1}",
                        parent.Kind,
                        result.GetType().Name);
                    throw new Exception(sb.ToString(), ex);
                }

                try
                {
                    IMgaFCO fco = model.CreateChildObject(role);
                    result.Impl = fco as IMgaObject;
                    return result;
                }
                catch (Exception ex)
                {
                    StringBuilder sb = new StringBuilder();
                    sb.AppendLine("New element could not be created.");
                    sb.AppendFormat("Container type: {0} Requested role: {1}",
                        parent.Kind,
                        result.GetType().Name);
                    throw new Exception(sb.ToString(), ex);
                }
            }
            else if (parent.Impl is MgaFolder)
            {
                try
                {
                    MgaFolder folder = parent.Impl as MgaFolder;

                    MgaMetaFolder item = folder.MetaFolder.
                        LegalChildFolders.
                        Cast<MgaMetaFolder>().
                        FirstOrDefault(x => x.MetaRef == metaRef);

                    if (item != null)
                    {
                        // create new folder
                        MgaFolder f = folder.CreateFolder(item);
                        result.Impl = f as IMgaObject;
                        return result;
                    }
                    else
                    {
                        MgaMetaFCO itemFco = folder.MetaFolder.
                            LegalRootObjects.
                            Cast<MgaMetaFCO>().
                            FirstOrDefault(x => x.MetaRef == metaRef);

                        if (itemFco != null)
                        {
                            IMgaFCO f = folder.CreateRootObject(itemFco);
                            result.Impl = f as IMgaObject;
                            return result;
                        }
                    }
                }
                catch (Exception ex)
                {
                    StringBuilder sb = new StringBuilder();
                    sb.AppendLine("New element could not be created in folder.");
                    sb.AppendFormat("Container type: {0} Requested child type: {1}",
                        parent.Kind,
                        result.GetType().Name);
                    throw new Exception(sb.ToString(), ex);
                }
            }

            return null;
        }

        public static T MakeConnection<T>(
            Interfaces.FCO src,
            Interfaces.FCO dst,
            Interfaces.Reference srcRef = null,
            Interfaces.Reference dstRef = null,
            Interfaces.Container parent = null,
            string roleStr = null)
            where T: Classes.Connection, new()
        {
            Contract.Requires(src != null);
            Contract.Requires(dst != null);

            IMgaFCO connection = null;
            
            T result = new T();

            if (parent == null)
            {
                try
                {
                    if (srcRef == null &&
                            dstRef == null)
                    {
                        // set the parent if it is null
                        if (src.ParentContainer.Impl == dst.ParentContainer.Impl)
                        {
                            parent = src.ParentContainer;
                        }
                        else if (src.ParentContainer.Impl == dst.ParentContainer.ParentContainer.Impl)
                        {
                            parent = src.ParentContainer;
                        }
                        else if (src.ParentContainer.ParentContainer.Impl == dst.ParentContainer.Impl)
                        {
                            parent = dst.ParentContainer;
                        }
                        else if (src.ParentContainer.ParentContainer.Impl == dst.ParentContainer.ParentContainer.Impl &&
                                         src.ParentContainer.Impl != dst.ParentContainer.Impl)
                        {
                            parent = src.ParentContainer.ParentContainer;
                        }
                    }
                    else if (srcRef != null &&
                                     dstRef == null)
                    {
                        if (srcRef.ParentContainer.Impl == dst.ParentContainer.Impl)
                        {
                            parent = dst.ParentContainer;
                        }
                        else if (srcRef.ParentContainer.Impl == dst.ParentContainer.ParentContainer.Impl)
                        {
                            parent = srcRef.ParentContainer;
                        }
                    }
                    else if (srcRef == null &&
                                     dstRef != null)
                    {
                        if (src.ParentContainer.Impl == dstRef.ParentContainer.Impl)
                        {
                            parent = src.ParentContainer;
                        }
                        else if (src.ParentContainer.ParentContainer.Impl == dstRef.ParentContainer.Impl)
                        {
                            parent = dstRef.ParentContainer;
                        }
                    }
                    else if (srcRef != null &&
                                     dstRef != null)
                    {
                        if (srcRef.ParentContainer.Impl == dstRef.ParentContainer.Impl)
                        {
                            parent = srcRef.ParentContainer;
                        }
                    }
                    if (parent == null)
                    {
                        throw new Exception("Parent could not be identified based on the given parameters.");
                    }
                }
                catch (NullReferenceException ex)
                {
                    // handle exception here
                    throw ex;
                }
            }

            if (parent.Impl is MgaModel)
            {
                MgaModel model = parent.Impl as MgaModel;
                MgaMetaRole role = null;

                try
                {
                    if (string.IsNullOrEmpty(roleStr))
                    {
                        if (result.GetType().Name != typeof(Interfaces.Connection).Name)
                        {
                            try
                            {
                                role = (model.MetaBase as MgaMetaModel).RoleByName[result.GetType().Name];
                            }
                            catch (Exception ex)
                            {
                                StringBuilder sb = new StringBuilder();
                                sb.AppendLine("Role was not found in the container.");
                                sb.AppendLine("Paradigm violation.");
                                sb.AppendFormat("Container type: {0} Requested role: {1}",
                                    parent.Kind,
                                    result.GetType().Name);
                                throw new Exception(sb.ToString(), ex);
                            }
                        }
                        else
                        {
                            // use default role
                            string srcKind = src.Kind;
                            string dstKind = dst.Kind;
                            MgaMetaRoles roles = (model.MetaBase as MgaMetaModel).Roles;
                            foreach (MgaMetaRole item in roles)
                            {
                                MgaMetaBase metaBase = item.MetaProject.FindObject[item.Kind.MetaRef];
                                if (metaBase is MgaMetaConnection)
                                {
                                    foreach (IMgaMetaConnJoint joint in (metaBase as MgaMetaConnection).Joints)
                                    {
                                        IEnumerable<MgaMetaPointerItem> srcSpecs = joint.PointerSpecByName["src"].Items.Cast<MgaMetaPointerItem>();
                                        IEnumerable<MgaMetaPointerItem> dstSpecs = joint.PointerSpecByName["dst"].Items.Cast<MgaMetaPointerItem>();
                                        if (srcSpecs.FirstOrDefault(x => x.Desc == srcKind) != null &&
                                            dstSpecs.FirstOrDefault(x => x.Desc == dstKind) != null)
                                        {
                                            // role found (first)
                                            role = item;
                                            break;
                                        }
                                    }
                                    if (role != null)
                                    {
                                        // role is ok
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        try
                        {
                            // try to use user defined role
                            role = (model.MetaBase as MgaMetaModel).RoleByName[roleStr];
                        }
                        catch (Exception ex)
                        {
                            StringBuilder sb = new StringBuilder();
                            sb.AppendLine("Role was not found in the container.");
                            sb.AppendLine("Paradigm violation.");
                            sb.AppendFormat("Container type: {0} Requested role: {1}",
                                parent.Kind,
                                result.GetType().Name);
                            throw new Exception(sb.ToString(), ex);
                        }
                    }

                    if (role == null)
                    {
                        throw new Exception("Role not found based on the given parameters.");
                    }
                    if (srcRef == null &&
                                dstRef == null)
                    {
                        connection = model.CreateSimpleConnDisp(
                            role,
                            src.Impl as MgaFCO,
                            dst.Impl as MgaFCO,
                            null,
                            null);
                    }
                    else if (srcRef != null &&
                                     dstRef == null)
                    {
                        connection = model.CreateSimpleConnDisp(
                            role,
                            src.Impl as MgaFCO,
                            dst.Impl as MgaFCO,
                            srcRef.Impl as MgaFCO,
                            null);
                    }
                    else if (srcRef == null &&
                                     dstRef != null)
                    {
                        connection = model.CreateSimpleConnDisp(
                            role,
                            src.Impl as MgaFCO,
                            dst.Impl as MgaFCO,
                            null,
                            dstRef.Impl as MgaFCO);
                    }
                    else if (srcRef != null &&
                        dstRef != null)
                    {
                        connection = model.CreateSimpleConnDisp(
                            role,
                            src.Impl as MgaFCO,
                            dst.Impl as MgaFCO,
                            srcRef.Impl as MgaFCO,
                            dstRef.Impl as MgaFCO);
                    }
                }
                catch (Exception ex)
                {
                    throw ex;
                }
            }
            else
            {
                throw new Exception("Parent could not be a folder.");
            }

            result.Impl = connection;

            if (result.Impl == null)
            {
                return null;
            }
            else
            {
                return result;
            }
        }


        /// <summary>
        /// 
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="subject"></param>
        /// <returns></returns>
        public static T CreateObject<T>(IMgaObject subject)
            where T : ISIS.GME.Common.Classes.Base, new()
        {
            // TODO: rename this function
            Contract.Requires(subject != null, "Subject cannot be null.");

            ISIS.GME.Common.Interfaces.Base newObject = null;

            //if (IsObjectCacheEnabled)
            //{
            //  if (ObjectCache.TryGetValue(subject, out newObject) == false)
            //  {
            //    // create new object and put it into the cache
            //    newObject = new T() { Impl = subject as IMgaObject };
            //    ObjectCache.Add(subject, newObject);
            //  }
            //}
            //else
            {
                newObject = new T() { Impl = subject as IMgaObject };
            }
            return newObject as T;
        }

        public static string ReplaceNames(this string oldName)
        {
            return oldName.
                Replace('(', '_').
                Replace(')', '_').
                Replace('/', '_').
                Replace("-", "_dash_").
                Replace(".", "_dot_").
                Replace(":", "_").
                Replace(' ', '_').
                Replace("&", "_and_");
        }

        //public static bool IsObjectCacheEnabled = true;

        //public static Dictionary<IMgaObject, ISIS.GME.Common.Interfaces.Base> ObjectCache =
        //	new Dictionary<IMgaObject, ISIS.GME.Common.Interfaces.Base>();

        public static IEnumerable<TResult> CastSrcConnections<TResult, TSource>(
            this TSource source, string kind = "")
            where TResult : ISIS.GME.Common.Classes.Base, new()
            where TSource : IMgaFCO
        {
            Contract.Requires(source != null, "Subject cannot be null.");

            foreach (MgaConnPoint cp in source.PartOfConns)
            {
                MgaSimpleConnection simple = cp.Owner as MgaSimpleConnection;
                if (simple != null)
                {
                    if (simple.Dst == source as IMgaFCO)
                    {
                        TResult result = new TResult() { Impl = simple as IMgaObject };

                        if (string.IsNullOrEmpty(kind))
                        {
                            yield return result;
                        }
                        else if (simple.MetaBase.Name == kind)
                        {
                            yield return result;
                        }
                    }
                }
            }
        }

        public static IEnumerable<TResult> CastDstConnections<TResult, TSource>(
            this TSource source, string kind = "")
            where TResult : ISIS.GME.Common.Classes.Base, new()
            where TSource : IMgaFCO
        {
            Contract.Requires(source != null, "Subject cannot be null.");

            foreach (MgaConnPoint cp in source.PartOfConns)
            {
                MgaSimpleConnection simple = cp.Owner as MgaSimpleConnection;
                if (simple != null)
                {
                    if (simple.Src == source as IMgaFCO)
                    {
                        TResult result = new TResult() { Impl = simple as IMgaObject };

                        if (string.IsNullOrEmpty(kind))
                        {
                            yield return result;
                        }
                        else if (simple.MetaBase.Name == kind)
                        {
                            yield return result;
                        }
                    }
                }
            }
        }

        public static TResult CastSrcEndRef<TResult, TSource>(
            this TSource source, string kind = "")
            where TResult : ISIS.GME.Common.Classes.Base, new()
            where TSource : IMgaFCO
        {
            Contract.Requires(source != null, "Subject cannot be null.");
            Contract.Requires(source is MgaSimpleConnection, "Subject must be a MgaSimpleConnection");
            Contract.Requires((source as MgaSimpleConnection).Src != null, "Src cannot be null.");

            MgaSimpleConnection simple = source as MgaSimpleConnection;
            // TODO: use factory for valid casts
            if (kind == simple.Src.MetaBase.Name)
            {
                IMgaFCO Ref = simple.SrcReferences.Cast<IMgaFCO>().FirstOrDefault();
                if (Ref != null)
                {
                    TResult result = new TResult() { Impl = Ref as IMgaObject };
                    return result;
                }
                else
                {
                    return null;
                }
            }
            else if (string.IsNullOrEmpty(kind))
            {
                IMgaFCO Ref = simple.SrcReferences.Cast<IMgaFCO>().FirstOrDefault();
                if (Ref != null)
                {
                    TResult result = new TResult() { Impl = Ref as IMgaObject };
                    return result;
                }
                else
                {
                    return null;
                }
            }
            else
            {
                return null;
            }
        }

        public static TResult CastDstEndRef<TResult, TSource>(
            this TSource source, string kind = "")
            where TResult : ISIS.GME.Common.Classes.Base, new()
            where TSource : IMgaFCO
        {
            Contract.Requires(source != null);
            Contract.Requires(source is MgaSimpleConnection);
            Contract.Requires((source as MgaSimpleConnection).Dst != null);

            MgaSimpleConnection simple = source as MgaSimpleConnection;
            // TODO: use factory for valid casts
            if (kind == simple.Dst.MetaBase.Name)
            {
                IMgaFCO Ref = simple.DstReferences.Cast<IMgaFCO>().FirstOrDefault();
                if (Ref != null)
                {
                    TResult result = new TResult() { Impl = Ref as IMgaObject };
                    return result;
                }
                else
                {
                    return null;
                }
            }
            else if (string.IsNullOrEmpty(kind))
            {
                IMgaFCO Ref = simple.DstReferences.Cast<IMgaFCO>().FirstOrDefault();
                if (Ref != null)
                {
                    TResult result = new TResult() { Impl = Ref as IMgaObject };
                    return result;
                }
                else
                {
                    return null;
                }
            }
            else
            {
                return null;
            }
        }

        public static TResult CastSrcEnd<TResult, TSource>(
            this TSource source, string kind = "")
            where TResult : ISIS.GME.Common.Classes.Base, new()
            where TSource : IMgaFCO
        {
            Contract.Requires(source != null, "Subject cannot be null.");
            Contract.Requires(source is MgaSimpleConnection, "Subject must be a MgaSimpleConnection");
            Contract.Requires((source as MgaSimpleConnection).Src != null, "Src cannot be null.");

            MgaSimpleConnection simple = source as MgaSimpleConnection;
            // TODO: use factory for valid casts
            if (kind == simple.Src.MetaBase.Name)
            {
                TResult result = new TResult() { Impl = simple.Src as IMgaObject };
                return result;
            }
            else if (string.IsNullOrEmpty(kind))
            {
                TResult result = new TResult() { Impl = simple.Src as IMgaObject };
                return result;
            }
            else
            {
                return null;
            }
        }

        public static TResult CastDstEnd<TResult, TSource>(
            this TSource source, string kind = "")
            where TResult : ISIS.GME.Common.Classes.Base, new()
            where TSource : IMgaFCO
        {
            Contract.Requires(source != null);
            Contract.Requires(source is MgaSimpleConnection);
            Contract.Requires((source as MgaSimpleConnection).Dst != null);

            MgaSimpleConnection simple = source as MgaSimpleConnection;
            // TODO: use factory for valid casts
            if (kind == simple.Dst.MetaBase.Name)
            {
                TResult result = new TResult() { Impl = simple.Dst as IMgaObject };
                return result;
            }
            else if (string.IsNullOrEmpty(kind))
            {
                TResult result = new TResult() { Impl = simple.Dst as IMgaObject };
                return result;
            }
            else
            {
                return null;
            }
        }

        public static IEnumerable<TResult> CastMgaChildren<TResult, TSource>(
            this TSource source, string kind = "")
            where TResult : ISIS.GME.Common.Classes.Base, new()
            where TSource : MgaObjects
        {
            Contract.Requires(source != null);

            IEnumerable<IMgaObject> children = source.Cast<IMgaObject>();
            ISIS.GME.Common.Interfaces.Base newObject = null;

            if (String.IsNullOrEmpty(kind))
            {
                // return all kinds
                foreach (IMgaObject v in source)
                {
                    newObject = new TResult() { Impl = v as IMgaObject };
                    yield return newObject as TResult;
                }
            }
            else
            {
                // return with the specifed kinds only
                foreach (IMgaObject v in children.Where(x => x.MetaBase.Name == kind))
                {
                    newObject = new TResult() { Impl = v as IMgaObject };
                    yield return newObject as TResult;
                }
            }
        }

        public static IEnumerable<TResult> CastMgaChildren<TResult, TSource>(
            this TSource source, int metaRef)
            where TResult : ISIS.GME.Common.Classes.Base, new()
            where TSource : MgaObjects
        {
            Contract.Requires(source != null);

            IEnumerable<IMgaObject> children = source.Cast<IMgaObject>();

            // return with the specifed kinds only
            foreach (IMgaObject v in children.Where(x => x.MetaBase.MetaRef == metaRef))
            {
                TResult newObject = new TResult() { Impl = v as IMgaObject };
                yield return newObject as TResult;
            }
        }

        public static IEnumerable<ISIS.GME.Common.Interfaces.Base> CastMgaChildren<TSource>(
            this TSource source, Dictionary<int, Type> metaRefTypes)
            where TSource : MgaObjects
        {
            Contract.Requires(source != null);

            IEnumerable<IMgaObject> children = source.Cast<IMgaObject>();

            // return with the specifed kinds only
            foreach (IMgaObject v in children)
            {
                ISIS.GME.Common.Classes.Base newObject = null;
                newObject = Activator.CreateInstance(metaRefTypes[v.MetaBase.MetaRef]) as ISIS.GME.Common.Classes.Base;
                newObject.Impl = v as IMgaObject;
                yield return newObject;
            }
        }


        public static IEnumerable<TResult> CastMgaObject<TResult, TSource>(
            this IEnumerable<TSource> source)
            where TResult : ISIS.GME.Common.Classes.Base, new()
            where TSource : IMgaObject
        {
            Contract.Requires(source != null);

            foreach (var v in source)
            {
                yield return new TResult() { Impl = v as IMgaObject };
            }
        }

        public static IEnumerable<TResult> CastMgaObject<TResult, TSource>(
            this TSource source)
            where TResult : ISIS.GME.Common.Classes.Base, new()
            where TSource : MgaObjects
        {
            return source.Cast<IMgaObject>().CastMgaObject<TResult, IMgaObject>();
        }


        /// <summary>
        /// Retrives only the specified type, which does NOT include
        /// the derived types.
        /// </summary>
        /// <typeparam name="TResult"></typeparam>
        /// <param name="source"></param>
        /// <returns></returns>
        public static IEnumerable<TResult> OfKind<TResult>(
            this IEnumerable<ISIS.GME.Common.Interfaces.Base> source)
            where TResult : ISIS.GME.Common.Interfaces.Base
        {
            string kind = typeof(TResult).Name;
            return source.Where(x => x.Kind == kind).Cast<TResult>();
        }


        public static int GetEnumItemNumber(IMgaFCO IMgaFCO, string attributeName)
        {
            Contract.Requires(IMgaFCO != null);
            Contract.Requires(IMgaFCO.MetaBase is MgaMetaFCO);
            Contract.Requires(string.IsNullOrEmpty(attributeName) == false);

            try
            {
                MgaMetaFCO meta = (IMgaFCO.MetaBase as MgaMetaFCO);
                MgaMetaAttribute attr = meta.AttributeByName[attributeName];

                string selectedItemValue = IMgaFCO.Attribute[attr].StringValue;
                int i = 0;
                foreach (MgaMetaEnumItem item in attr.EnumItems)
                {
                    if (item.Value == selectedItemValue)
                    {
                        return i;
                    }
                    i++;
                }
                throw new Exception("Attribute / selected item was not found.");
            }
            catch (Exception)
            {

                throw;
            }

        }

        public static void SetEnumItem(IMgaFCO IMgaFCO, string attributeName, int index)
        {
            Contract.Requires(IMgaFCO != null);
            Contract.Requires(IMgaFCO.MetaBase is MgaMetaFCO);
            Contract.Requires(string.IsNullOrEmpty(attributeName) == false);

            try
            {
                MgaMetaFCO meta = (IMgaFCO.MetaBase as MgaMetaFCO);
                MgaMetaAttribute attr = meta.AttributeByName[attributeName];

                IMgaFCO.Attribute[attr].StringValue = attr.EnumItems[index + 1].Value;
            }
            catch (Exception ex)
            {
                throw ex;
            }
        }

        public static ISIS.GME.Common.Interfaces.FCO CastReferred(
            MgaReference mgaReference,
            Dictionary<int, Type> metaRefTypes)
        {
            return CastReferred(mgaReference, metaRefTypes, "<unknown>");
        }

        public static ISIS.GME.Common.Interfaces.FCO CastReferred(
            MgaReference mgaReference,
            Dictionary<int, Type> metaRefTypes,
            string kind)
        {
            Contract.Requires(mgaReference != null);

            IMgaFCO referred = mgaReference.Referred;
            if (referred == null)
            {
                return null;
            }
            else
            {
                ISIS.GME.Common.Interfaces.FCO result = null;
                Type t;
                if (metaRefTypes.TryGetValue(referred.MetaBase.MetaRef, out t) == false)
                {
                    // backwards compat dictates that this exception must be KeyNotFoundException
                    throw new System.Collections.Generic.KeyNotFoundException(String.Format("Tried to retrieve Referred.{0}, but the referred object is \"{1}\", which does not inherit from \"{0}\"", kind, referred.MetaBase.Name));
                }
                result = Activator.CreateInstance(t) as ISIS.GME.Common.Interfaces.FCO;
                (result as ISIS.GME.Common.Classes.FCO).Impl = referred as IMgaObject;
                return result;
            }
        }


        public static IEnumerable<T> CastSetMembers<T>(
            MgaSet mgaSet,
            string kind = null)
            where T : ISIS.GME.Common.Classes.FCO, new()
        {
            Contract.Requires(mgaSet != null);

            if (typeof(T) != typeof(ISIS.GME.Common.Classes.FCO))
            {
                kind = typeof(T).Name;
            }

            if (string.IsNullOrEmpty(kind))
            {
                // return with all kind
                foreach (IMgaFCO item in mgaSet.Members)
                {
                    yield return new T() { Impl = item as IMgaObject };
                }
            }
            else
            {
                foreach (IMgaFCO item in mgaSet.Members)
                {
                    if (item.Meta.Name == kind)
                    {
                        yield return new T() { Impl = item as IMgaObject };
                    }
                }
            }
        }

        public static IEnumerable<T> MembersOfSet<T>(
            IMgaFCO mgaFco,
            string kind = null)
            where T : ISIS.GME.Common.Classes.Set, new()
        {
            Contract.Requires(mgaFco != null);

            if (typeof(T) != typeof(ISIS.GME.Common.Classes.Set))
            {
                kind = typeof(T).Name;
            }

            if (string.IsNullOrEmpty(kind))
            {
                // return with all kind
                foreach (IMgaFCO item in mgaFco.MemberOfSets)
                {
                    yield return new T() { Impl = item as IMgaObject };
                }
            }
            else
            {
                foreach (IMgaFCO item in mgaFco.MemberOfSets)
                {
                    if (item.Meta.Name == kind)
                    {
                        yield return new T() { Impl = item as IMgaObject };
                    }
                }
            }
        }

        public static IEnumerable<Interfaces.Reference> ReferencedBy<T>(
            IMgaFCO mgaFco,
            string kind = null)
            where T : ISIS.GME.Common.Classes.Reference, new()
        {
            Contract.Requires(mgaFco != null);

            if (typeof(T) != typeof(ISIS.GME.Common.Classes.Reference))
            {
                kind = typeof(T).Name;
            }

            if (string.IsNullOrEmpty(kind))
            {
                // return with all kind
                foreach (IMgaFCO item in mgaFco.ReferencedBy)
                {
                    yield return new T() { Impl = item as IMgaObject };
                }
            }
            else
            {
                foreach (IMgaFCO item in mgaFco.ReferencedBy)
                {
                    if (item.Meta.Name == kind)
                    {
                        yield return new T() { Impl = item as IMgaObject };
                    }
                }
            }
        }

        public static void SetReferred(
            MgaReference Impl,
            ISIS.GME.Common.Interfaces.FCO value)
        {
            Contract.Requires(Impl != null);
            Contract.Requires(Impl is MgaReference);

            try
            {
                if (value != null)
                {
                    (Impl as MgaReference).Referred = value.Impl as MgaFCO;
                }
                else
                {
                    (Impl as MgaReference).Referred = null;
                }
            }
            catch (Exception ex)
            {
                throw new Exception("Referred value could not be set.", ex);
            }
        }

        public static IEnumerable<ISIS.GME.Common.Interfaces.FCO> CastReferencedBy(
            IMgaFCO IMgaFCO,
            Dictionary<int, Type> dictionary)
        {
            Contract.Requires(IMgaFCO != null);

            int metaRef;
            ISIS.GME.Common.Interfaces.FCO fco = null;

            foreach (IMgaFCO item in IMgaFCO.ReferencedBy)
            {
                try
                {
                    metaRef = item.MetaBase.MetaRef;
                    Type type = null;
                    if (dictionary.TryGetValue(metaRef, out type))
                    {
                        // can be casted to the requested type
                        fco = Activator.CreateInstance(type) as ISIS.GME.Common.Interfaces.FCO;
                        (fco as ISIS.GME.Common.Classes.FCO).Impl = item as IMgaObject;
                    }
                    else
                    {
                        // cannot be casted to DSML types
                        continue;
                    }
                }
                catch (Exception ex)
                {
                    throw ex;
                }
                yield return fco;
            }
        }

        public static IEnumerable<ISIS.GME.Common.Interfaces.FCO> CastSetMembers(
            MgaSet mgaSet,
            Dictionary<int, Type> dictionary)
        {
            Contract.Requires(mgaSet != null);


            //List<ISIS.GME.Common.Interfaces.FCO> result = new List<Interfaces.FCO>();
            int metaRef;
            ISIS.GME.Common.Interfaces.FCO fco = null;
            Type t = null;
            foreach (IMgaFCO item in mgaSet.Members)
            {
                metaRef = item.MetaBase.MetaRef;
                if (dictionary.TryGetValue(metaRef, out t))
                {
                    fco = Activator.CreateInstance(t) as ISIS.GME.Common.Interfaces.FCO;
                    (fco as ISIS.GME.Common.Classes.FCO).Impl = item as IMgaObject;
                    yield return fco;
                }
            }
        }

        public static IEnumerable<Interfaces.FCO> CastMembersOfSet(
            IMgaFCO mgaFco,
            Dictionary<int, Type> dictionary)
        {
            Contract.Requires(mgaFco != null);

            int metaRef;
            ISIS.GME.Common.Interfaces.FCO fco = null;
            Type t = null;
            foreach (IMgaFCO item in mgaFco.MemberOfSets)
            {
                metaRef = item.MetaBase.MetaRef;
                if (dictionary.TryGetValue(metaRef, out t))
                {
                    fco = Activator.CreateInstance(t) as ISIS.GME.Common.Interfaces.FCO;
                    (fco as ISIS.GME.Common.Classes.FCO).Impl = item as IMgaObject;
                    yield return fco;
                }
            }
        }
    }
}
