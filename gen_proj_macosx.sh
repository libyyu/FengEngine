self_path=$(cd `dirname $0`; pwd)
cd $self_path

platform=macosx
arch=x86_64
xmake f -c -p $platform -a $arch --genproj=y --FengEngineBundle=y
xmake project -k xcode -a $arch -m "release,debug" sln/$platform