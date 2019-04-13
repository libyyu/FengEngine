:: AUTO GENERATOR BY XMAKE, DO'NOT MODIFY.
@echo off
@IF NOT DEFINED NDK_HOME GOTO :NO_NDK_HOME
@rem gen FengEngine
@echo Compiling NativeCode... FengEngine
"%NDK_HOME%\ndk-build.cmd" NDK_PROJECT_PATH=. NDK_APPLICATION_MK=ApplicationDebug.mk
@if errorlevel 1 goto :BAD
goto :SUCCESS

:NO_NDK_HOME
@echo.
@echo Build [FengEngine] Error, NDK_HOME NOT DEFINED!
@echo.
@pause
exit /B 1
:BAD
@echo.
@echo Build [FengEngine] Error!
@echo.
@pause
exit /B 1


:SUCCESS
@echo.
@echo Build [FengEngine] Done!
exit /B 0
