using System;
using System.Collections.Generic;
using System.Text;
using GME.MGA;
using System.Linq;
using ISIS.GME.Dsml.BidirConnection.Interfaces;

namespace DsmlGeneratorTest
{
    class Program
    {
        static void AssertEqual<T>(IEnumerable<T> model, IEnumerable<string> names)
            where T : ISIS.GME.Common.Interfaces.Base
        {
            IEnumerable<string> modelNames = model.Select(m => m.Name).OrderBy(name => name);
            if (Enumerable.SequenceEqual(
                modelNames,
                names) == false)
            {
                throw new Exception(String.Format("Expected {1}. Got {0}", String.Join(" ", modelNames.ToArray()), String.Join(" ", names.ToArray())));
            }
        }

        static int Main(string[] args)
        {
            try
            {
                MgaProject project = new MgaProject();
                bool ro_mode;
                project.Open(args[0], out ro_mode);
                project.BeginTransactionInNewTerr();
                try
                {
                    RootFolder rf = ISIS.GME.Dsml.BidirConnection.Classes.RootFolder.GetRootFolder(project);
                    var children = rf.Children.ModelCollection.GetEnumerator();
                    children.MoveNext();
                    Model model = children.Current;
                    AssertEqual(model.Children.ModelCollection, new string[] { "Child1", "Child2" });
                    foreach (Model child in model.Children.ModelCollection)
                    {
                        var conns = new string[] { "C1_C2", "C2_C1" };
                        AssertEqual(child.SrcConnections.ConnectionCollection, conns);
                        AssertEqual(child.DstConnections.ConnectionCollection, conns);
                        AssertEqual(child.AllDstConnections, conns);
                        AssertEqual(child.AllSrcConnections, conns);
                    }
                }
                finally
                {
                    project.AbortTransaction();
                    project.Close();
                }
            }
            catch (Exception e)
            {
                Console.Error.WriteLine(e.ToString());
                return 11;
            }
            return 0;
        }
    }
}
