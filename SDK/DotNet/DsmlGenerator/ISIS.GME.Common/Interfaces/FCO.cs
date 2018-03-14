using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ISIS.GME.Common.Interfaces
{
	/// <summary>
	/// Represents the generic domain independent FCO.
	/// </summary>
	public interface FCO : Base
	{
		ISIS.GME.Common.Interfaces.FCO ArcheType
		{
			get;
		}

		bool IsInstance
		{
			get;
		}

		bool IsSubtype
		{
			get;
		}

		IEnumerable<Interfaces.Set> GenericMembersOfSet
		{
			get;
		}

		IEnumerable<Interfaces.Reference> GenericReferencedBy
		{
			get;
		}

		IEnumerable<ISIS.GME.Common.Interfaces.Connection> AllSrcConnections
		{
			get;
		}

		IEnumerable<ISIS.GME.Common.Interfaces.Connection> AllDstConnections
		{
			get;
		}
    }
}
