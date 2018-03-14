using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ISIS.GME.Common.Interfaces
{
	/// <summary>
	/// Represents the generic domain independent Connection.
	/// </summary>
	public interface Connection : FCO
	{
		/// <summary>
		/// Domain independent source end of the connection
		/// </summary>
		ISIS.GME.Common.Interfaces.FCO GenericSrcEnd
		{
			get;
		}

		/// <summary>
		/// Domain independent destination end of the connection
		/// </summary>
		ISIS.GME.Common.Interfaces.FCO GenericDstEnd
		{
			get;
		}

		/// <summary>
		/// <para>
		/// Domain independent source reference of the connection.
		/// </para>
		/// <para>
		/// Gets the reference parent on the source side of the connection.
		/// </para>
		/// </summary>
		ISIS.GME.Common.Interfaces.Reference GenericSrcEndRef
		{
			get;
		}

		/// <summary>
		/// <para>
		/// Domain independent destination reference of the connection.
		/// </para>
		/// <para>
		/// Gets the reference parent on the destination side of the connection.
		/// </para>
		/// </summary>
		ISIS.GME.Common.Interfaces.Reference GenericDstEndRef
		{
			get;
		}

		/// <summary>
		/// Contains the preferences
		/// </summary>
		ISIS.GME.Common.Classes.PreferencesConnection Preferences
		{
			get;
		}

		/// <summary>
		/// Contains the derived instances.
		/// </summary>
		IEnumerable<ISIS.GME.Common.Interfaces.Connection> GenericInstances
		{
			get;
		}

		/// <summary>
		/// Contains the derived subtypes.
		/// </summary>
		IEnumerable<ISIS.GME.Common.Interfaces.Connection> GenericSubTypes
		{
			get;
		}
	}
}
