self_path=$(cd `dirname $0`; pwd)
cd $self_path


platform=iphoneos
arch=x86_64
xmake f -c -p $platform -a $arch || exit 1
xmake $* || exit 1
cp -r libs/iphoneos/x86_64/libFengEngine.a /Users/lidengfeng/Documents/Workspace/PokeGame/ProjectUnity/Assets/Plugins/iOS/