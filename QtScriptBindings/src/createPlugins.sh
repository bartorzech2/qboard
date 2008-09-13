#!/bin/bash
########################################################################
# Quick hack to mangle the qtscriptgenerator-generated code a bit
# for use as a plugin of my own.
prefix=com_trolltech_qt_
modules="core gui network opengl sql svg uitools webkit xml xmlpatterns"

for m in $modules; do
    dir=$prefix$m
    echo "Generating build stuff for ${dir}..."
    cd $dir &>/dev/null || {
	echo "Cannot cd to $dir!"
	exit 1
    }
    {
    echo "PLUGIN_NAME=${dir}"
    case $m in
	xml|xmlpatterns)
	    echo "QT += xml"
	    echo 'QMAKE_CXXFLAGS += -I$${QMAKE_INCDIR_QT}/QtXmlPatterns'
	    ;;
	uitools)
	    echo "QT += gui"
	    echo 'QMAKE_LIBS += -lQtUiTools'
	    ;;
	webkit)
	    echo "QT += webkit network"
	    ;;
	*)
	    echo "QT += ${m}"
	    ;;
    esac
    echo 'include(../plugin_template.qmake)'
    } > ${dir}.pro
    if false; then
    if [[ -f main.cpp ]]; then
	list=$(grep -l "<quiloader.h>" *.cpp *.h 2>/dev/null)
	if [[ x != "${list}x" ]]; then
	    echo "Replacing <quiloader.h> ..."
	    perl -i -pe 's|<quiloader.h>|<QtUiTools/quiloader.h>|' $list
	fi
	pat='/class com/,/^};$/'
	hpp=${prefix}${m}_ScriptPlugin.h
    #sed -n -e "${pat}w ${hpp}" < main.cpp
	sed -e "${pat}d" < main.cpp > plugin.cpp
	lineno=$(grep -n '#include' plugin.cpp |tail -1 | cut -d: -f1)
	lineno=$((lineno + 1))
    #echo "Found insert point at line $lineno"
	perl -i -ne "print \"#include \\\"${hpp}\\\"\n\" if \$. == ${lineno}; print;" plugin.cpp
    else
	echo "Not hacking up main.cpp"
    fi
    fi
    cd - >/dev/null
done
