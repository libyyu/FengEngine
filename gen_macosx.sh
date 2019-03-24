self_path=$(cd `dirname $0`; pwd)
cd $self_path


platform=macosx
arch=x86_64
xmake f -c -p $platform -a $arch --FengEngineBundle=y || exit 1
xmake $* || exit 1
rm -rf /Users/lidengfeng/Documents/Workspace/PokeGame/ProjectUnity/Assets/Plugins/MacOSX/
mkdir -p /Users/lidengfeng/Documents/Workspace/PokeGame/ProjectUnity/Assets/Plugins/MacOSX
cp -r bin/macosx/x86_64/FengEngine.bundle /Users/lidengfeng/Documents/Workspace/PokeGame/ProjectUnity/Assets/Plugins/MacOSX/FengEngine.bundle