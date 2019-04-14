self_path=$(cd `dirname $0`; pwd)
cd $self_path

platform=android
NDK_HOME=/Users/lidengfeng/Library/Android/android-ndk-r19c
dest_dir=/Users/lidengfeng/Documents/Workspace/PokeGame/ProjectUnity/Assets/Plugins/Android/libs

# arch=armv7-a
# xmake f -c -p $platform -a $arch --ndk=$NDK_HOME || exit 1
# xmake $* || exit 1
# mkdir -p $dest_dir/armeabi-v7a
# cp -rf bin/$platform/$arch/libFengEngine.so $dest_dir/armeabi-v7a/

arch=x86
xmake f -c -p $platform -a $arch --ndk=$NDK_HOME -v -D || exit 1
xmake $* || exit 1
mkdir -p $dest_dir/x86
cp -rf bin/$platform/$arch/libFengEngine.so $dest_dir/x86/
cp -rf bin/$platform/$arch/libFengEngine.so /Users/lidengfeng/Documents/Workspace/testapk2/FLuaGame/src/main/jniLibs/x86/

xmake project -k mk -a "armv7-a,x86" -m "release,debug" sln/android