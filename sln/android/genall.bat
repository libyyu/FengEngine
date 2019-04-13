:: AUTO GENERATOR BY XMAKE, DO'NOT MODIFY.
@echo off

@set SELF_PATH=%~dp0
@set SELF_PATH=%SELF_PATH:~,-1%
cd %SELF_PATH%
@echo current dir: %SELF_PATH%

@echo check NDK_HOME
@IF NOT DEFINED NDK_HOME GOTO :NO_NDK_HOME

@echo gen lua51 -- genApplicationRelease
cd lua51
call genApplicationRelease.bat
@if errorlevel 1 goto :BAD
cd ..


@echo gen lua51 -- genApplicationDebug
cd lua51
call genApplicationDebug.bat
@if errorlevel 1 goto :BAD
cd ..


@echo gen luasocket -- genApplicationRelease
cd luasocket
call genApplicationRelease.bat
@if errorlevel 1 goto :BAD
cd ..


@echo gen luasocket -- genApplicationDebug
cd luasocket
call genApplicationDebug.bat
@if errorlevel 1 goto :BAD
cd ..


@echo gen cjson -- genApplicationRelease
cd cjson
call genApplicationRelease.bat
@if errorlevel 1 goto :BAD
cd ..


@echo gen cjson -- genApplicationDebug
cd cjson
call genApplicationDebug.bat
@if errorlevel 1 goto :BAD
cd ..


@echo gen thirdpart -- genApplicationRelease
cd thirdpart
call genApplicationRelease.bat
@if errorlevel 1 goto :BAD
cd ..


@echo gen thirdpart -- genApplicationDebug
cd thirdpart
call genApplicationDebug.bat
@if errorlevel 1 goto :BAD
cd ..


@echo gen FengEngine -- genApplicationRelease
cd FengEngine
call genApplicationRelease.bat
@if errorlevel 1 goto :BAD
cd ..


@echo gen FengEngine -- genApplicationDebug
cd FengEngine
call genApplicationDebug.bat
@if errorlevel 1 goto :BAD
cd ..


@echo gen TestLua -- genApplicationRelease
cd TestLua
call genApplicationRelease.bat
@if errorlevel 1 goto :BAD
cd ..


@echo gen TestLua -- genApplicationDebug
cd TestLua
call genApplicationDebug.bat
@if errorlevel 1 goto :BAD
cd ..


goto :SUCCESS


:NO_NDK_HOME
@echo.
@echo Build Error,NO_NDK_HOME NOT FOUND!
@pause
@echo.
exit /B 1


:BAD
@echo.
@echo Build Error!
@echo.
@pause
exit /B 1


:SUCCESS
@echo.
@echo Build Done!
exit /B 0
