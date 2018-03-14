using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ISIS.GME.Common.Interfaces
{
	/// <summary>
	/// Represents the generic domain independent Container.
	/// </summary>
	public interface Container : Base
	{
		IEnumerable<ISIS.GME.Common.Interfaces.Base> AllChildren
		{
			get;
		}
	}
}
