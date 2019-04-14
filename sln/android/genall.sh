## AUTO GENERATOR BY XMAKE, DO'NOT MODIFY.
echo ""
SELF_PATH=$(cd `dirname $0`; pwd)
cd $SELF_PATH
echo "current dir: $SELF_PATH"

if [ ! "$NDK_HOME" ]; then
	echo "Build Error, NDK_HOME NOT DEFINED!"
	exit 1
fi

echo "gen lua51 -- genApplicationRelease"
cd lua51
sh genApplicationRelease.sh || { echo "Build Error!"; exit 1; }
cd ..


echo "gen lua51 -- genApplicationDebug"
cd lua51
sh genApplicationDebug.sh || { echo "Build Error!"; exit 1; }
cd ..


echo "gen luasocket -- genApplicationRelease"
cd luasocket
sh genApplicationRelease.sh || { echo "Build Error!"; exit 1; }
cd ..


echo "gen luasocket -- genApplicationDebug"
cd luasocket
sh genApplicationDebug.sh || { echo "Build Error!"; exit 1; }
cd ..


echo "gen cjson -- genApplicationRelease"
cd cjson
sh genApplicationRelease.sh || { echo "Build Error!"; exit 1; }
cd ..


echo "gen cjson -- genApplicationDebug"
cd cjson
sh genApplicationDebug.sh || { echo "Build Error!"; exit 1; }
cd ..


echo "gen thirdpart -- genApplicationRelease"
cd thirdpart
sh genApplicationRelease.sh || { echo "Build Error!"; exit 1; }
cd ..


echo "gen thirdpart -- genApplicationDebug"
cd thirdpart
sh genApplicationDebug.sh || { echo "Build Error!"; exit 1; }
cd ..


echo "gen FengEngine -- genApplicationRelease"
cd FengEngine
sh genApplicationRelease.sh || { echo "Build Error!"; exit 1; }
cd ..


echo "gen FengEngine -- genApplicationDebug"
cd FengEngine
sh genApplicationDebug.sh || { echo "Build Error!"; exit 1; }
cd ..


echo "Build Done!"
exit 0
