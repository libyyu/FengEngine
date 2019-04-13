## AUTO GENERATOR BY XMAKE, DO'NOT MODIFY.
if [ ! "$NDK_HOME" ]; then
	echo "Build [FengEngine] Error, NDK_HOME NOT DEFINED!"
	exit 1
fi
# gen FengEngine
echo "Compiling NativeCode... FengEngine"
if [ "`uname -s`" = "Darwin" ]; then
	$NDK_HOME/ndk-build NDK_PROJECT_PATH=. NDK_APPLICATION_MK=ApplicationDebug.mk $* || { echo "Build [FengEngine] Error!"; exit 1; }
else
	$NDK_HOME/ndk-build.cmd NDK_PROJECT_PATH=. NDK_APPLICATION_MK=ApplicationDebug.mk $* || { echo "Build [FengEngine] Error!"; exit 1; }
fi
echo "Build [FengEngine] Done!"
exit 0
