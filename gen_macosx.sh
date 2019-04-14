self_path=$(cd `dirname $0`; pwd)
cd $self_path

dest_dir=/Users/lidengfeng/Documents/Workspace/PokeGame/ProjectUnity/Assets/Plugins/MacOSX
platform=macosx
arch=x86_64
xmake f -c -p $platform -a $arch --FengEngineBundle=y || exit 1
xmake $* || exit 1
rm -rf $dest_dir/
mkdir -p $dest_dir
cp -r bin/macosx/x86_64/FengEngine.bundle $dest_dir/FengEngine.bundle