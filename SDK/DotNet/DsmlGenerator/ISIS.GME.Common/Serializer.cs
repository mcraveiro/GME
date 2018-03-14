using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace ISIS.GME.Common
{
	public static class Serializer
	{
		public enum Type
		{
			/// <summary>
			/// Calls the serializer only on this object.
			/// </summary>
			OnlyThisObject,
			
			/// <summary>
			/// Calls the serializer on all child objects.
			/// </summary>
			Children,

			/// <summary>
			/// Calls the serializer on the subgraph, which starts with this object.
			/// </summary>
			SubGraph,
		}
	}
}
