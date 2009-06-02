#!/bin/bash
########################################################################
# A quick hack to generate a source dist file for QBoard.

VERSION=$(date +%Y%m%d)

DEST=libS11nQt-1.2.9-${VERSION}
echo "Creating $DEST ..."


test -d ${DEST} && rm -fr ${DEST}
mkdir ${DEST}
test -d ${DEST} || {
    echo "Destination dir [${DEST}] could not be created!"
    exit 1
}
find . -name '*~' | xargs rm -f

sdirs="s11n app include"

echo "Copying files..."
for d in $sdirs; do
    echo -e "\t... $d"
    find $d \
	-type f \
	-name '*.h' \
	-o -name '*.?pp' \
	-o -name '*.pro' \
	| sed -e '/\/bak/d' \
	-e '/\/nono/d' \
	-e '/\.#/d' \
	-e '/\.svn/d' \
	> filelist
    cp --parents $(cat filelist) $DEST || {
	err=$?
	echo "Copy failed!"
	rm filelist
	exit $err
    }
    rm filelist

    #echo cp --parents $(set -x; find $d $nameargs)
done

cp \
    README.txt \
    LICENSE.txt \
    config.qmake \
    S11nQt.pro \
    $DEST

echo "Kludging in static version number..."

echo "Removing a few unwanted files..."
find $DEST -name 'qrc_*.cpp' | xargs rm -f
find $DEST -type d -name nono -o -name release -o -name debug | xargs rm -fr
find $DEST -type d -name  debug -o -name release | xargs rm -fr

echo "Tarring...";
TF=$DEST.tar.bz2
tar cjf $TF $DEST
echo "Cleaning up..."
rm -fr $DEST

echo "Done:"
ls -l $TF
