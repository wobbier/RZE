@echo off
REM using crappy stuff here until a better build pipeline exists -- this serves its purpose atm

set debugExecPath=%~dp0_Build\Debug\x64\
set debugAssetPath=%~dp0_Build\Debug\x64\Assets
set debugConfigPath=%~dp0_Build\Debug\x64\Config
set releaseExecPath=%~dp0_Build\Release\x64\
set releaseAssetPath=%~dp0_Build\Release\x64\Assets
set releaseConfigPath=%~dp0_Build\Release\x64\Config

if exist %debugExecPath% (
	if not exist %debugAssetPath% (mkdir %debugAssetPath%)
	if not exist %debugConfigPath% (mkdir %debugConfigPath%)
	
	xcopy /y /d /s /f %~dp0Assets %debugAssetPath%
	xcopy /y /d /s /f %~dp0Config %debugConfigPath%
)
if exist %releaseExecPath% (
	if not exist %releaseAssetPath% (mkdir %releaseAssetPath%)
	if not exist %releaseConfigPath% (mkdir %releaseConfigPath%)
	
	xcopy /y /d /s /f %~dp0Assets %releaseAssetPath%
	xcopy /y /d /s /f %~dp0Config %releaseConfigPath%
)