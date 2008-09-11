#!/bin/bash
########################################################################
# A quick hack to generate function bodies for member functions of
# QScriptable types.

function usage()
{
cat <<EOF
Usage: $0 [-h] [-c] ClassName

Generates some code (to stdout) for member functions.

Use -h to generate header declarations. Use -c to generate
implementation code.

Input must come from stdin in the form of lines, each containing:

return_type|functionName|arguments

e.g. for this member function:

Foo * foo(double a1, QString const & arg2);

Feed us this line:

Foo *|foo|double a1, QString const & arg2

Each line is converted into a header decl and/or a C++ implementation
for the specified class.

If you need const members, the const specifier will need to be added
by hand.
EOF
exit 1
}

opt_header=0
opt_cpp=0
for i in "$@"; do
    case "$i" in
	-h) opt_header=1
	continue
	;;
	-c) opt_cpp=1
	continue
	;;
	*)
	class="$i"
	break
	;;
    esac
done
test x = "x${class}" && usage

namespace=qboard

IFS='|'
while read RV FN PARAMS; do
    if [[ x1 = x${opt_header} ]]; then
cat <<EOF
    /**
        See ${class}::${FN}() here.
    */
    Q_INVOKABLE ${RV} ${FN}(${PARAMS});
EOF
    fi
    if [[ x1 = x${opt_cpp} ]]; then
cat <<EOF
    ${RV} ${class}::${FN}(${PARAMS})
    {
	SELF(${RV}());
	return self->${FN}(${PARAMS});
    }
EOF
    fi
done
