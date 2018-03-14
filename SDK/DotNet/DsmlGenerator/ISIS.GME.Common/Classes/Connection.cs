using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GME.MGA;
using System.Diagnostics.Contracts;

namespace ISIS.GME.Common.Classes
{
	public class Connection : FCO, ISIS.GME.Common.Interfaces.Connection
	{
		public static Interfaces.Connection ConnectGeneric(
			Interfaces.FCO src,
			Interfaces.FCO dst,
			Interfaces.Reference srcRef = null,
			Interfaces.Reference dstRef = null,
			Interfaces.Container parent = null,
			string role = null)
		{
			return ISIS.GME.Common.Utils.MakeConnection<Classes.Connection>(
				src,
				dst,
				srcRef,
				dstRef,
				parent,
				role);
		}


		public virtual ISIS.GME.Common.Interfaces.FCO SrcEnd
		{
			get
			{
				throw new NotSupportedException(
					"This property must be implemented in the derived class.");
			}
		}

		public virtual ISIS.GME.Common.Interfaces.FCO DstEnd
		{
			get
			{
				throw new NotSupportedException(
					"This property must be implemented in the derived class.");
			}
		}

		public virtual ISIS.GME.Common.Interfaces.FCO GenericSrcEnd
		{
			get
			{
				return Utils.CastSrcEnd<FCO, IMgaFCO>(Impl as IMgaFCO);
			}
		}

		public virtual ISIS.GME.Common.Interfaces.FCO GenericDstEnd
		{
			get
			{
				return Utils.CastDstEnd<FCO, IMgaFCO>(Impl as IMgaFCO);
			}
		}

		public virtual ISIS.GME.Common.Interfaces.Reference GenericSrcEndRef
		{
			get
			{
				return Utils.CastSrcEndRef<Reference, IMgaFCO>(Impl as IMgaFCO);
			}
		}

		public virtual ISIS.GME.Common.Interfaces.Reference GenericDstEndRef
		{
			get
			{
				return Utils.CastDstEndRef<Reference, IMgaFCO>(Impl as IMgaFCO);
			}
		}


		public PreferencesConnection Preferences
		{
			get { return new PreferencesConnection(Impl as global::GME.MGA.IMgaFCO); }
		}

		/// <summary>
		/// 
		/// </summary>
		public virtual IEnumerable<ISIS.GME.Common.Interfaces.Connection> GenericInstances
		{
			get
			{
				Contract.Requires(Impl != null);
				Contract.Requires(Impl is IMgaFCO);

				foreach (IMgaFCO item in (Impl as IMgaFCO).DerivedObjects)
				{
					if (item.IsInstance)
					{
						ISIS.GME.Common.Classes.Connection result = new Connection();
						result.Impl = item as IMgaObject;
						yield return result;
					}
				}
			}
		}

		/// <summary>
		/// 
		/// </summary>
		public virtual IEnumerable<ISIS.GME.Common.Interfaces.Connection> GenericSubTypes
		{
			get
			{
				Contract.Requires(Impl != null);
				Contract.Requires(Impl is IMgaFCO);

				foreach (IMgaFCO item in (Impl as IMgaFCO).DerivedObjects)
				{
					if (item.IsInstance ? false : item.ArcheType != null)
					{
						// if subtype
						ISIS.GME.Common.Classes.Connection result = new Connection();
						result.Impl = item as IMgaObject;
						yield return result;
					}
				}
			}
		}


		public override void Serialize(
			ISIS.GME.Common.IndentedStreamWriter writer,
			Serializer.Type type = Serializer.Type.OnlyThisObject)
		{
			// serialize this object
			StringBuilder sb = new StringBuilder();
			string SrcRefName = "null";
			string DstRefName = "null";
			if (GenericSrcEndRef != null)
			{
				SrcRefName = GenericSrcEndRef.Name;
			}
			if (GenericDstEndRef != null)
			{
				DstRefName = GenericDstEndRef.Name;
			}

			sb.AppendFormat(
				"{0} [{1} -> {2}] [SrcRef = {3} DstRef = {4}]",
				Name,
				GenericSrcEnd.Name,
				GenericDstEnd.Name,
				SrcRefName,
				DstRefName);

			writer.WriteLine(sb.ToString(), true);

			if ((type == Serializer.Type.SubGraph ||
				type == Serializer.Type.Children) &&
				this is Interfaces.Container)
			{
				writer.Depth++;
				// serialize child objects
				foreach (Interfaces.Base b in (this as Interfaces.Container).AllChildren)
				{
					if (type == Serializer.Type.SubGraph)
					{
						b.Serialize(writer, type);
					}
					else if (type == Serializer.Type.Children)
					{
						b.Serialize(writer, Serializer.Type.OnlyThisObject);
					}
				}
				writer.Depth--;
			}
		}
	}
}
