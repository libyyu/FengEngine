## AUTO GENERATOR BY XMAKE, DO'NOT MODIFY.
if [ ! "$NDK_HOME" ]; then
	echo "Build [lua51] Error, NDK_HOME NOT DEFINED!"
	exit 1
fi
# gen lua51
echo "Compiling NativeCode... lua51"
if [ "`uname -s`" = "Darwin" ]; then
	$NDK_HOME/ndk-build NDK_PROJECT_PATH=. NDK_APPLICATION_MK=ApplicationDebug.mk $* || { echo "Build [lua51] Error!"; exit 1; }
else
	$NDK_HOME/ndk-build.cmd NDK_PROJECT_PATH=. NDK_APPLICATION_MK=ApplicationDebug.mk $* || { echo "Build [lua51] Error!"; exit 1; }
fi
echo "Build [lua51] Done!"
exit 0
