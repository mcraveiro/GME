using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ISIS.GME.Common
{
	/// <summary>
	/// Helps to write indented output file.
	/// </summary>
	public class IndentedStreamWriter : System.IO.StreamWriter
	{
		public uint NumIndentChar { get; set; }
		public char IndentChar { get; set; }

		public uint Depth { get; set; }

		public void WriteLine(string text, bool indent)
		{
			StringBuilder sb = new StringBuilder();
			if (indent)
			{
				for (uint i = 0; i < NumIndentChar * Depth; ++i)
				{
					sb.Append(IndentChar);
				}
			}
			sb.Append(text);
			WriteLine(sb.ToString());
		}

		public IndentedStreamWriter(string path)
			: base(path)
		{
			NumIndentChar = 2;
			IndentChar = ' ';
			Depth = 0;
		}
	}
}
