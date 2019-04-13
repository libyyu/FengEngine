:: AUTO GENERATOR BY XMAKE, DO'NOT MODIFY.
@echo off
@IF NOT DEFINED NDK_HOME GOTO :NO_NDK_HOME
@rem gen luasocket
@echo Compiling NativeCode... luasocket
"%NDK_HOME%\ndk-build.cmd" NDK_PROJECT_PATH=. NDK_APPLICATION_MK=ApplicationRelease.mk
@if errorlevel 1 goto :BAD
goto :SUCCESS

:NO_NDK_HOME
@echo.
@echo Build [luasocket] Error, NDK_HOME NOT DEFINED!
@echo.
@pause
exit /B 1
:BAD
@echo.
@echo Build [luasocket] Error!
@echo.
@pause
exit /B 1


:SUCCESS
@echo.
@echo Build [luasocket] Done!
exit /B 0
