Readme for GME.DSMLGenerator NuGet package

Description:

This NuGet package will instrument your C# project to generate and compile domain-specific code. The resulting .NET assembly is an API for your MetaGME language.

Requirements/instructions:

The paradigm .mga file must reside in the same directory as the .csproj file. The filenames (without extensions) must be the same, and must be the same as the Root Folder's name.

To use the generated API, create another C# (or other .NET language) project, add the NuGet package GME.DSMLGenerator.Runtime, and add a reference to the other project.

GME <http://repo.isis.vanderbilt.edu/GME/> must be installed to run.

Known Issues:

This message results when you don't have a ProjectName.mga file in the same directory as your .csproj file:
    Error	1	The "GenerateCSharpDSML" task failed unexpectedly.
    System.Exception: Error generating DSML ---> System.IO.FileNotFoundException: The system cannot find the file specified.

If your paradigm has namespaces, you must manually add the .cs files for the namespaces to the project.

Bugs:

Please report bugs to <http://escher.isis.vanderbilt.edu/JIRA/browse/GME> or gme-supp@isis.vanderbilt.edu