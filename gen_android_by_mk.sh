self_path=$(cd `dirname $0`; pwd)
cd $self_path

platform=android
NDK_HOME=/Users/lidengfeng/Library/Android/android-ndk-r19c
dest_dir=/Users/lidengfeng/Documents/Workspace/PokeGame/ProjectUnity/Assets/Plugins/Android/libs

xmake f -c -p $platform -a armv7-a --ndk=$NDK_HOME || exit 1
xmake project -k mk -a "armv7-a,x86" -m "release,debug" sln/android || exit 1

sh sln/android/genall.sh || exit 1
src_dir=sln/android/FengEngine
mkdir -p $dest_dir/armeabi-v7a
mkdir -p $dest_dir/x86
cp -rf $src_dir/obj/local/armeabi-v7a/libFengEngine.so $dest_dir/armeabi-v7a/
cp -rf $src_dir/obj/local/x86/libFengEngine.so $dest_dir/x86/

# test
testapk=/Users/lidengfeng/Documents/Workspace/testapk3
cp -rf $src_dir/obj/local/armeabi-v7a/libFengEngine.so $testapk/FLuaGame/src/main/jniLibs/armeabi-v7a/
cp -rf $src_dir/obj/local/x86/libFengEngine.so $testapk/FLuaGame/src/main/jniLibs/x86/
