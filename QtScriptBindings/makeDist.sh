#!/bin/bash
########################################################################
# A quick hack to generate a source dist file for QBoard.

VERSION=$(date +%Y%m%d)

DEST=QtScriptBindings-${VERSION}
echo "Creating $DEST ..."


test -d ${DEST} && rm -fr ${DEST}
mkdir ${DEST}
test -d ${DEST} || {
    echo "Destination dir [${DEST}] could not be created!"
    exit 1
}
find . -name '*~' | xargs rm -f

sdirs="src"

echo "Copying files..."
for d in $sdirs; do
    echo -e "\t... $d"
    find $d \
	-type f \
	-name '*.h' \
	-o -name '*.?pp' \
	-o -name '*.pr?' \
	-o -name '*.js' \
	-o -name '*.qs' \
	-o -name '*.qmake' \
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

if [[ -e doc ]]; then
    echo "Including docs in dist (not part of svn tree)"
    cp -Lrp doc $DEST
else
    echo "NOT INCLUDING DOCS in dist! To get them, copy the qtscriptgenerator doc dir here."
fi
cp -rp examples $DEST

cp ChangeLog \
    README.txt \
    LICENSE.GPL? \
    LICENSE.txt \
    QtScriptBindings.pro \
    $DEST

echo "Removing a few unwanted files..."
find $DEST -name 'qrc_*.cpp' -o -name '*~' | xargs rm -f
find $DEST -name nono -o -name release -o -name debug | xargs rm -fr
find $DEST -name  debug -o -name release | xargs rm -fr


echo "Tarring...";
TF=$DEST.tar.bz2
tar cf - $DEST | bzip2 > $TF
echo "Cleaning up..."
rm -fr $DEST

echo "Done:"
ls -l $TF
