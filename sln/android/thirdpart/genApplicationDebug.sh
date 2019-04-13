## AUTO GENERATOR BY XMAKE, DO'NOT MODIFY.
if [ ! "$NDK_HOME" ]; then
	echo "Build [thirdpart] Error, NDK_HOME NOT DEFINED!"
	exit 1
fi
# gen thirdpart
echo "Compiling NativeCode... thirdpart"
if [ "`uname -s`" = "Darwin" ]; then
	$NDK_HOME/ndk-build NDK_PROJECT_PATH=. NDK_APPLICATION_MK=ApplicationDebug.mk $* || { echo "Build [thirdpart] Error!"; exit 1; }
else
	$NDK_HOME/ndk-build.cmd NDK_PROJECT_PATH=. NDK_APPLICATION_MK=ApplicationDebug.mk $* || { echo "Build [thirdpart] Error!"; exit 1; }
fi
echo "Build [thirdpart] Done!"
exit 0
