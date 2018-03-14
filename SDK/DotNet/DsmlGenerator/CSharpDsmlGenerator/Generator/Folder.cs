using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.CodeDom;
using GME.MGA;

namespace CSharpDSMLGenerator.Generator
{
	public class Folder : Base
	{
		public Folder(
			MgaObject subject,
			CodeTypeReferenceCollection baseTypes,
			CodeTypeReferenceCollection attributes) :
				base(subject, baseTypes, attributes)
		{
		}
	}
}
