using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using System.IO;
using System.Xml;
using System.Diagnostics.Contracts;

namespace MgaMeta
{
	public class DsmlModel
	{

		public paradigm Paradigm { get; set; }
		public DateTime ParadigmDate { get; set; }

		/// <summary>
		/// Reads the paradigm definition from an xmp file.
		/// Note: DTD validition is turned off.
		/// </summary>
		/// <param name="filename"></param>
		public DsmlModel(string filename)
		{
			Paradigm = GetParadigm(filename);
			ParadigmDate = File.GetLastWriteTime(filename);
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="kindName"></param>
		/// <returns>0 if it is not defined</returns>
		public int GetMetaRef(string kindName)
		{
			Contract.Requires(string.IsNullOrEmpty(kindName) == false);
			Contract.Requires(Paradigm != null);

			if (Paradigm.folder.name == kindName)
			{
				return int.Parse(Paradigm.folder.metaref);
			}

			if (Paradigm.folder.folder1 != null)
			{
				foreach (var item in Paradigm.folder.folder1)
				{
					if (item.name == kindName)
					{
						return int.Parse(item.metaref);
					}
				}
			}

			foreach (var item in Paradigm.folder.Items)
			{
				if (item is atom)
				{
					if ((item as atom).name == kindName)
					{
						return int.Parse((item as atom).metaref);
					}
				}
				else if (item is connection)
				{
					if ((item as connection).name == kindName)
					{
						return int.Parse((item as connection).metaref);
					}
				}
				else if (item is set)
				{
					if ((item as set).name == kindName)
					{
						return int.Parse((item as set).metaref);
					}
				}
				else if (item is reference)
				{
					if ((item as reference).name == kindName)
					{
						return int.Parse((item as reference).metaref);
					}
				}
				else if (item is model)
				{
					if ((item as model).name == kindName)
					{
						return int.Parse((item as model).metaref);
					}
				}
			}
			//StringBuilder sb = new StringBuilder();
			//sb.AppendFormat("MetaRef was not found for kind name: {0}", kindName);
			//throw new InvalidDataException(sb.ToString());
			// by default if it is not found
			return 0;
		}

		public int GetChildRoleRef(
			string parentKindName,
			string childKindName)
		{

			foreach (var item in Paradigm.folder.Items.OfType<model>())
			{
				if (item.name == parentKindName)
				{
					foreach (var r in item.role)
					{
						if (r.kind == childKindName)
						{
							return int.Parse(r.metaref);
						}
					}
				}
			}
			//throw new InvalidDataException();
			return 0;
		}

        public int GetChildRoleRef(
            string parentKindName,
            string childKindName,
            string roleName)
        {

            foreach (var item in Paradigm.folder.Items.OfType<model>())
            {
                if (item.name == parentKindName)
                {
                    foreach (var r in item.role)
                    {
                        if (r.kind == childKindName && r.name == roleName)
                        {
                            return int.Parse(r.metaref);
                        }
                    }
                }
            }
            //throw new InvalidDataException();
            return 0;
        }

        public static paradigm GetParadigm(string filename)
		{
			XmlReaderSettings settings = new XmlReaderSettings()
			{
				DtdProcessing = DtdProcessing.Ignore,
				ValidationType = ValidationType.DTD,

			};

			paradigm result = null;
			XmlSerializer xs = new XmlSerializer(typeof(paradigm));
			using (XmlReader reader = XmlReader.Create(filename, settings))
			{
				result = (paradigm)xs.Deserialize(reader);
			}
			return result;
		}

	}
}
