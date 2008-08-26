#!/bin/bash
########################################################################
# A quick hack to generate a source dist file for QBoard.

VERSION=$(date +%Y%m%d)

DEST=QBoard-${VERSION}
echo "Creating $DEST ..."


test -d ${DEST} && rm -fr ${DEST}
mkdir ${DEST}
test -d ${DEST} || {
    echo "Destination dir [${DEST}] could not be created!"
    exit 1
}
find . -name '*~' | xargs rm -f

sdirs="apps counters resources src s11n ui plugins"

echo "Copying files..."
for d in $sdirs; do
    echo -e "\t... $d"
    find $d \
	-name '*.h' \
	-o -name '*.?pp' \
	-o -name '*.pro' \
	-o -name '*.ui' \
	-o -name '*.qrc' \
	-o -name '*.png' \
	-o -name '*.html' \
	| sed -e '/\/bak/d' \
	-e '/\/nono/d' \
	-e '/\/debug/d' \
	-e '/\/release/d' \
	-e '/\.#/d' \
	-e '/\.svn/d' \
	-e '/qrc_*.cpp/d' \
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

cp INSTALL.* \
    ChangeLog \
    LICENSE.GPL? \
    LICENSE.txt \
    config.qmake \
    QBoard.pro \
    $DEST

echo "Kludging in static version number..."

perl -pe "s|^\s*QBOARD_VERSION.+|QBOARD_VERSION=${VERSION}|" < config.qmake > $DEST/config.qmake


echo "Removing a few unwanted files..."
find $DEST -name 'qrc_*.cpp' | xargs rm -f
find $DEST -name nono -o -name release -o -name debug | xargs rm -fr

echo "Tarring...";
TF=$DEST.tar.bz2
tar cjf $TF $DEST
echo "Cleaning up..."
rm -fr $DEST

echo "Done:"
ls -l $TF
