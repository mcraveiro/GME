param($installPath, $toolsPath, $package, $project)

# Set-PsDebug -trace 2

Import-Module (Join-Path $toolsPath "Remove.psm1")
Remove-Changes $project

# $project is a EnvDTE.Project
# $project.Object is a VSLangProj.VSProject

$absolutePath = Join-Path $toolsPath "DsmlGenerator.targets"
$absoluteUri = New-Object -typename System.Uri -argumentlist $absolutePath
$projectUri = New-Object -typename System.Uri -argumentlist $project.FullName
$relativeUri = $projectUri.MakeRelativeUri($absoluteUri)
$relativePath = [System.URI]::UnescapeDataString($relativeUri.ToString()).Replace([System.IO.Path]::AltDirectorySeparatorChar, [System.IO.Path]::DirectorySeparatorChar)

# Need to load MSBuild assembly if it's not loaded yet.
Add-Type -AssemblyName 'Microsoft.Build, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b03f5f7f11d50a3a'
$msbuild = [Microsoft.Build.Evaluation.ProjectCollection]::GlobalProjectCollection.GetLoadedProjects($project.FullName) | Select-Object -First 1

$msbuild.Xml.AddImport($relativePath)

$ref = $project.Object.References.Find("CSharpDSMLGenerator")
if ($ref) { $ref.Remove() }

$project.Save()
