#!/bin/bash

if [[ "$1x" = "x" ]]; then
    echo "Usage: $0 QT_VERSION"
    exit 1
fi

DEST=alt-$1

test -d $DEST && rm -fr $DEST
mkdir ${DEST}
test -d ${DEST} || {
    echo "Destination dir [${DEST}] could not be created!"
    exit 1
}

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
	-o -name '*.js' \
	-o -name '*.html' \
	| sed -e '/\/bak/d' \
	-e '/\/nono/d' \
	-e '/\/release/d' \
	-e '/\/debug/d' \
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
done

cd $DEST && {
    orig=$PWD/..

    for i in config.qmake QBoard.pro; do
	ln -s ../$i .
    done
    dirs=$(find . -type d -name '[a-zA-Z1-9]*')
    echo dirs=${dirs}
    for d in $dirs; do
	cd $d
	echo "==> $PWD"
	for f in *; do
	test -L $f && continue
	test -d $f && continue
	rm $f
	ln -s $orig/$d/$f .
	echo -e "\tLinked $f"
	done
	echo "<== $PWD"
	cd -
    done
}
cd -
