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


build f -c -p $PLATFORM --xcode_dir="$XCODE_DIR" --file=$self_path/xm.lua $PARAMS
build --file=$self_path/xm.lua


