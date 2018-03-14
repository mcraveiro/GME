using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GME.MGA;
using System.Diagnostics.Contracts;

namespace CSharpDSMLGenerator.Generator
{
	public static class Configuration
	{
		//public static Dictionary<string, int> LocalFactory = new Dictionary<string, int>();

		public static MgaMeta.DsmlModel DsmlModel { get; set; }

		public const bool IsPartial = false;

		public static bool UseOnlyBaseForFunctions { get { return true; } }

		public const string GmeNamespaceDelimiter = "::";

		public const string BaseNamespace = "ISIS.GME";
		public const string DsmlNamespaceSuffix = "Dsml";

		public const string Classes = "Classes";
		public const string Interfaces = "Interfaces";

		public const string DsmlNamespace =
				BaseNamespace + "." +
				DsmlNamespaceSuffix;

		public static string DsmlName { get; set; }

		public static string ProjectNamespace
		{
			get { return String.Format("{0}.{1}", DsmlNamespace, DsmlName); }
		}

		public static string ProjectClassNamespace
		{
			get { return String.Format("{0}.{1}", ProjectNamespace, Classes); }
		}

		public static string ProjectIntefaceNamespace
		{
			get { return String.Format("{0}.{1}", ProjectNamespace, Interfaces); }
		}

		/* Support for namespaces tricky, we need some modification in the code...
		public static string GetClassName(MgaFCO subject)
		{
			throw new NotImplementedException();
		}

		public static string GetInterfaceName(MgaFCO subject)
		{
			throw new NotImplementedException();
		}
		*/

		public struct Generator
		{
			public const string EnumPrefix = "_";
		}


		/// <summary>
		/// These comments will be included in the generated code.
		/// </summary>
		public struct Comments
		{
            public const string Empty =
@"<summary>
<para></para>
<para></para>
</summary>";

			public const string ArcheType =
@"<summary>
<para>the object that is at the farthest position within the chain of base objects (i.e. the one which is not derived from anything).</para>
<para> NULL if the object is not derived.</para>
</summary>";

			public const string AllReferred =
@"<summary>
<para></para>
<para></para>
</summary>";

			public const string Referred =
@"<summary>
<para></para>
<para></para>
</summary>";

			public const string AllDstConnections =
@"<summary>
<para>Contains the domain specific destination end point of this connection.</para>
<para></para>
</summary>";

			public const string AllSrcConnections =
@"<summary>
<para>Contains the domain specific source end point of this connection.</para>
<para></para>
</summary>";

			public const string AllReferencedBy =
@"<summary>
<para></para>
<para></para>
</summary>";

			public const string ReferencedBy =
@"<summary>
<para></para>
<para></para>
</summary>";

			public const string BaseType =
@"<summary>
<para></para>
<para></para>
</summary>";

			public const string IsInstance =
@"<summary>
<para></para>
<para></para>
</summary>";

			public const string IsSubtype =
@"<summary>
<para></para>
<para></para>
</summary>";

			public const string AllChildren =
@"<summary>
<para>Contains all the domain specific child objects.</para>
<para></para>
</summary>";

			public const string Children =
@"<summary>
<para>Contains the domain specific child objects.</para>
<para></para>
</summary>";

			public const string Attributes =
@"<summary>
<para>Contains the domain specific attributes.</para>
<para></para>
</summary>";

			public const string LibraryCollection =
@"<summary>
<para>Contains the domain specific attached libraries.</para>
<para></para>
</summary>";

			public const string LibraryName =
@"<summary>
<para>Contains the library's connection string if this object is an attached library.</para>
<para></para>
</summary>";

			public const string Roles =
@"<summary>
<para>WARNING: This part is still under developement.</para>
<para></para>
</summary>";

            public const string SrcConnections =
@"<summary>
<para>Retrieves all connections, which have this object as a DESTINATION.</para>
<para></para>
</summary>";

            public const string DstConnections =
@"<summary>
<para>Retrieves all connections, which have this object as a SOURCE.</para>
<para></para>
</summary>";

            public const string SrcConnectionEnd =
@"<summary>
<para>Contains the domain specific source end point of this connection.</para>
<para></para>
</summary>";

            public const string DstConnectionEnd =
@"<summary>
<para>Contains the domain specific destination end point of this connection.</para>
<para></para>
</summary>";

            public const string ParentContainer =
@"<para>Represents the domain specific parent container.</para>
<para></para>";
		}

		internal static string GetKindName(MgaObject obj)
		{
			string ns = GetNamespaceName(obj);
			if (string.IsNullOrEmpty(ns) == false)
			{
				ns = ns + GmeNamespaceDelimiter;
			}
			if (obj is MgaReference)
			{
				return ns + (obj as MgaReference).Referred.Name;
			}
			else
			{
				return ns + obj.Name;
			}

		}


		internal static string GetClassName(MgaObject obj)
		{
			Contract.Requires(obj != null);
			Contract.Requires(obj is MgaFCO);

			string ns = GetNamespaceName(obj);
			if (string.IsNullOrEmpty(ns) == false)
			{
				ns = "." + ns;
			}
			if (obj is MgaReference)
			{
				return Configuration.ProjectNamespace + ns + "." + Classes + "." + (obj as MgaReference).Referred.Name;
			}
			else
			{
				return Configuration.ProjectNamespace + ns + "." + Classes + "." + obj.Name;
			}
		}

		private static string GetNamespaceName(MgaObject obj)
		{
			Contract.Requires(obj != null);
			string ns ="";
			MgaModel paradigmSheet = null;
			MgaFolder rootFolder = null;

			if (obj.MetaBase.Name == "RootFolder")
			{
				rootFolder = obj as MgaFolder;
			}
			else
			{
				paradigmSheet = (obj as MgaFCO).ParentModel;
				rootFolder = GetLibraryRoot(paradigmSheet.ParentFolder);
			}
			try
			{
				ns = rootFolder.RegistryValue["Namespace"];
			}
			catch
			{
				// com exception...
			}
			return ns;
		}
		/// <summary>
		/// format NSName::
		/// </summary>
		/// <param name="obj"></param>
		/// <returns></returns>
		public static string GetGmeNamespaceName(MgaObject obj)
		{
			string ns = GetNamespaceName(obj);
			if (string.IsNullOrEmpty(ns) == false)
			{
				ns = ns + GmeNamespaceDelimiter;
			}
			return ns;
		}
		private static MgaFolder GetLibraryRoot(MgaFolder mgaFolder)
		{
			Contract.Requires(mgaFolder != null);
			if (mgaFolder.MetaBase.Name == "RootFolder")
			{
				return mgaFolder;
			}
			else
			{
				return GetLibraryRoot(mgaFolder.ParentFolder);
			}
		}

		internal static string GetInterfaceName(MgaObject obj)
		{
			Contract.Requires(obj != null);

			string ns = GetNamespaceName(obj);
			if (string.IsNullOrEmpty(ns) == false)
			{
				ns = "." + ns;
			}
			if (obj is MgaReference)
			{
				return Configuration.ProjectNamespace + ns + "." + Interfaces + "." + (obj as MgaReference).Referred.Name;
			}
			else
			{
				return Configuration.ProjectNamespace + ns + "." + Interfaces + "." + obj.Name;
			}
		}

		internal static string GetClassNamespace(MgaObject obj)
		{
			Contract.Requires(obj != null);

			string ns = GetNamespaceName(obj);
			if (string.IsNullOrEmpty(ns) == false)
			{
				ns = "." + ns;
			}
			if (obj is MgaReference)
			{
				return Configuration.ProjectNamespace + ns + "." + Classes;
			}
			else
			{
				return Configuration.ProjectNamespace + ns + "." + Classes;
			}
		}

		internal static string GetInterfaceNamespace(MgaObject obj)
		{
			Contract.Requires(obj != null);

			string ns = GetNamespaceName(obj);
			if (string.IsNullOrEmpty(ns) == false)
			{
				ns = "." + ns;
			}
			if (obj is MgaReference)
			{
				return Configuration.ProjectNamespace + ns + "." + Interfaces;
			}
			else
			{
				return Configuration.ProjectNamespace + ns + "." + Interfaces;
			}
		}

        internal static string GetNamespacePrefixedName(MgaObject obj)
        {
            string ns = GetNamespaceName(obj);
            if (string.IsNullOrEmpty(ns) == false)
            {
                return ns + "_" + obj.Name;
            }
            return obj.Name;
        }

        internal static IEnumerable<Tuple<MgaFCO, string>> GetUniqueNames(IEnumerable<MgaFCO> children, int count)
        {
            IEnumerable<Tuple<MgaFCO, string>> childNames;
            HashSet<string> names = new HashSet<string>(children.Distinct().Select(x => x.Name));
            if (names.Count != count)
            {
                childNames = children.Distinct().GroupBy(child => child.Name).Select(g => g.Count() > 1 ?
                    g.Select(x => new Tuple<MgaFCO, string>(x, Configuration.GetNamespacePrefixedName((MgaObject)x))) :
                    g.Select(x => new Tuple<MgaFCO, string>(x, x.Name))).SelectMany(child => child);
            }
            else
            {
                childNames = children.Distinct().Select(child => new Tuple<MgaFCO, string>(child, child.Name));
            }
            return childNames;
        }

		internal static string GetClassName(MgaFCO item)
		{
			return GetClassName(item as MgaObject);
		}

        internal static string GetClassName(string p)
        {
            return p.Replace("::", ".Classes.");
        }
    }
}
