$ErrorActionPreference = 'Stop'
$Root = (Resolve-Path (Join-Path $PSScriptRoot '..')).Path
$Output = Join-Path $Root 'GoatranceGater-source.zip'
$Items = @('CMakeLists.txt', 'README.md', 'LICENSE', 'Source', 'scripts') | ForEach-Object { Join-Path $Root $_ }
if (Test-Path $Output) { Remove-Item $Output }
Compress-Archive -Path $Items -DestinationPath $Output -Force
Write-Host "Created $Output"
