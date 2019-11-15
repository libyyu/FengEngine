## AUTO GENERATOR BY XMAKE, DO'NOT MODIFY.
if [ ! "$NDK_HOME" ]; then
	echo "Build [TestLua] Error, NDK_HOME NOT DEFINED!"
	exit 1
fi
# gen TestLua
echo "Compiling NativeCode... TestLua"
if [ "`uname -s`" = "Darwin" ]; then
	$NDK_HOME/ndk-build NDK_PROJECT_PATH=. NDK_APPLICATION_MK=ApplicationRelease.mk $* || { echo "Build [TestLua] Error!"; exit 1; }
else
	$NDK_HOME/ndk-build.cmd NDK_PROJECT_PATH=. NDK_APPLICATION_MK=ApplicationRelease.mk $* || { echo "Build [TestLua] Error!"; exit 1; }
fi
echo "Build [TestLua] Done!"
exit 0
