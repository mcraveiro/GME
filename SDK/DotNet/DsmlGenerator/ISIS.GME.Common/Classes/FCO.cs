using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GME.MGA;
using ISIS.GME.Common.Interfaces;

namespace ISIS.GME.Common.Classes
{
	/// <summary>
	/// Represents the generic domain independent FCO.
	/// </summary>
	public class FCO : Base, ISIS.GME.Common.Interfaces.FCO
	{
		/// <summary>
		/// Note: If you write domain specific code, do NOT use this.
		/// </summary>
		public ISIS.GME.Common.Interfaces.FCO ArcheType
		{
			get
			{
				if ((Impl as IMgaFCO).ArcheType == null)
				{
					return null;
				}
				else
				{
					return Utils.CreateObject<FCO>(Impl);
				}
			}
		}


		/// <summary>
		/// Note: If you write domain specific code, do NOT use this.
		/// </summary>
		public virtual IEnumerable<ISIS.GME.Common.Interfaces.Connection> AllSrcConnections
		{
			get
			{
				return Utils.CastSrcConnections<Connection, IMgaFCO>(Impl as IMgaFCO);
			}
		}

		/// <summary>
		/// Note: If you write domain specific code, do NOT use this.
		/// </summary>
		public virtual IEnumerable<ISIS.GME.Common.Interfaces.Connection> AllDstConnections
		{
			get
			{
				return Utils.CastDstConnections<Connection, IMgaFCO>(Impl as IMgaFCO);
			}
		}

		public bool IsInstance
		{
			get { return (Impl as IMgaFCO).IsInstance; }
		}

		public bool IsSubtype
		{
			get { return 
				(Impl as IMgaFCO).IsInstance ? 
				false : 
				(Impl as IMgaFCO).ArcheType != null; }
		}

		public IEnumerable<Interfaces.Set> GenericMembersOfSet
		{
			get { return Utils.MembersOfSet<Classes.Set>(Impl as IMgaFCO); }
		}

		public IEnumerable<Interfaces.Reference> GenericReferencedBy
		{
			get { return Utils.ReferencedBy<Classes.Reference>(Impl as IMgaFCO); }
		}

        ///// <summary>
        ///// This property must be implemented in the derived class.
        ///// </summary>
        //public virtual IEnumerable<ISIS.GME.Common.Interfaces.FCO> InstanceCollection
        //{
        //  get
        //  {
        //    throw new NotSupportedException(
        //      "This property must be implemented in the derived class.");
        //  }
        //}

        ///// <summary>
        ///// This property must be implemented in the derived class.
        ///// </summary>
        //public virtual IEnumerable<ISIS.GME.Common.Interfaces.FCO> DerivedCollection
        //{
        //  get
        //  {
        //    throw new NotSupportedException(
        //      "This property must be implemented in the derived class.");
        //  }
        //}

        ///// <summary>
        ///// This property must be implemented in the derived class.
        ///// </summary>
        //public virtual IEnumerable<ISIS.GME.Common.Interfaces.FCO> SubtypeCollection
        //{
        //  get
        //  {
        //    throw new NotSupportedException(
        //      "This property must be implemented in the derived class.");
        //  }
        //}

    }
}
