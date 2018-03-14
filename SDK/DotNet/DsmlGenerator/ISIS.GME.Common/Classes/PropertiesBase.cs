using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ISIS.GME.Common.Classes
{
	/// <summary>
	/// Contains the base set of properties.
	/// </summary>
	public class PropertiesBase
	{
		protected global::GME.MGA.IMgaObject Impl;

		/// <summary>
		/// Type of the object.
		/// </summary>
		public string Type { get { return Impl.ObjType.ToString(); } }

		/// <summary>
		/// Kind of the object.
		/// </summary>
		public string Kind { get { return Impl.MetaBase.Name; } }

		/// <summary>
		/// Unique ID of the object.
		/// </summary>
		public string ObjectId { get { return Impl.ID; } }

		/// <summary>
		/// Relative ID of the object, correspoonds with creation order.
		/// The object with grater relative ID was created later.
		/// </summary>
		public int RelativeId { get { return Impl.RelID; } }

		/// <summary>
		/// Meta ID of the object.
		/// </summary>
		public int MetaId { get { return Impl.MetaBase.MetaRef; } }

		/// <summary>
		/// Universally unique ID of the object.
		/// </summary>
		public Guid Guid { get { return new Guid(Impl.GetGuidDisp()); } }


		public PropertiesBase(global::GME.MGA.IMgaObject impl)
		{
			// TODO: Complete member initialization
			this.Impl = impl;
		}
	}
}
