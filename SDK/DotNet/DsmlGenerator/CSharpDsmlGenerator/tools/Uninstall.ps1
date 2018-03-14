param($installPath, $toolsPath, $package, $project)

Import-Module (Join-Path $toolsPath "Remove.psm1")

Remove-Changes $project

$project.Save()
