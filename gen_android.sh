self_path=$(cd `dirname $0`; pwd)
cd $self_path

NDK_HOME=/Users/lidengfeng/Library/Android/android-ndk-r19c
platform=android

arch=armv7-a
xmake f -c -p $platform -a $arch --ndk=$NDK_HOME || exit 1
xmake $* || exit 1
cp -r bin/$platform/$arch/libFengEngine.so /Users/lidengfeng/Documents/Workspace/PokeGame/ProjectUnity/Assets/Plugins/Android/libs/armeabi-v7a/

arch=x86
xmake f -c -p $platform -a $arch --ndk=$NDK_HOME || exit 1
xmake $* || exit 1
cp -r bin/$platform/$arch/libFengEngine.so /Users/lidengfeng/Documents/Workspace/PokeGame/ProjectUnity/Assets/Plugins/Android/libs/x86/