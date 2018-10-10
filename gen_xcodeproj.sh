self_path=$(cd `dirname $0`; pwd)
cd $self_path

quit()
{
	exit ${1+"$@"}
}
echo "current dir: $self_path"

build()
{
	xmake ${1+"$@"} || { echo>&2 "error build"; quit 1; }
}

XCODE_DIR=/Applications/Xcode.app
PLATFORM=macosx
PARAMS="$@"
echo "PARAMS: $PARAMS"


build f -c -p $PLATFORM -a x86_64 --xcode_dir="$XCODE_DIR" --file=$self_path/xm.lua  --genxcode=y --FengEngineBundle=y $PARAMS
build project -k xcode --file=$self_path/xm.lua -a x86_64  -m "release,debug" Projects/temp