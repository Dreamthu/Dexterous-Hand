# Windows entry point; use the Python environment with PyYAML installed.
$ErrorActionPreference = 'Stop'
& python (Join-Path $PSScriptRoot '../apps/offline_detection.py') run @args
exit $LASTEXITCODE
