#!/bin/bash
########################################################################
# A quick hack to generate QScriptable prototypes, and their supporting
# code, for the qboard project.

class="$1"

function usage()
{
cat <<EOF
Usage: $0 ClassName
ClassName must be a QObject type.
Generates some code for plugging in to the qboard script layer.
EOF
    exit 1
}

test x = "${class}x" && usage

echo "Creating QScript prototype and support code. It will not compile as-is. See the comments for details."

jsproto="JS${class}Prototype"
NS=qboard

cpp=${jsproto}.cpp
hpp=${jsproto}.h

test -e ${cpp} -o -e ${hpp} && {
    echo "ERROR: cannot overwrite existing file(s) $(ls ${cpp} ${hpp} 2>/dev/null). Delete them and try again."
    exit 2
}

funcfile=${jsproto}.funcs
if [[ -e ${funcfile} ]]; then
    echo "Using functions input file ${funcfile}"
else
    echo "Member functions file ${funcfile} not found. Skipping function generation."
    funcfile=
fi


########################################################################
# Header stuff...
{
cat <<EOF
#include <qboard/ScriptQt.h> // convert_script_value<> and to/fromScriptValue()
#include <QObject>
#include <QScriptable>

namespace ${NS} {
    /**
        Used by toScriptValue() and fromScriptValue() to convert to/from
	native- and JS-side ${class} objects.
    */
    template <>
    struct convert_script_value_f<${class}*>
    {
        QScriptValue operator()( QScriptEngine *, ${class} * const & r) const;
        ${class} * operator()( QScriptEngine *, const QScriptValue & args) const;
    };

    /**
        A JavaScript prototype class for wrapping ${class} objects
	in QtScript code.
    */
    class ${jsproto} : public QObject, public QScriptable
    {
	Q_OBJECT
    public:
	explicit ${jsproto}( QObject * parent = 0 );
	virtual ~${jsproto}();
	${class} * self();
EOF
if [[ x != "x${funcfile}" ]]; then
    ./createMemberFuncs.sh -h ${jsproto} < ${funcfile}
else
    cat <<EOF
	// Add Q_INVOKABLE or slot functions to make them accessible via script.
	Q_INVOKABLE int sampleFunction();
EOF
fi

cat <<EOF
    private:
	struct Impl;
	Impl * impl;
    };

} // namespace
EOF
} >  ${hpp}
########################################################################
# CPP stuff...
{
cat <<EOF
#include <${NS}/${hpp}>
//#include <${NS}/${class}.h>
Q_DECLARE_METATYPE(${NS}::${class}*);

#define IGNORE_NEXT_PART 0
#if IGNORE_NEXT_PART
// In some setup routine, where you initialize your JS engine...
// Register script conversions for ${class}:
qScriptRegisterMetaType(someJSEngine,
    qboard::toScriptValue<${class}*>,
    qboard::fromScriptValue<${class}*> );
// Register default prototype object for ${class}:
${NS}::${jsproto} * proto${class} = new ${NS}::${jsproto}( someJSEngine );
someJSEngine->setDefaultPrototype(qMetaTypeId<${class}*>(),
                                  someJSEngine->newQObject(proto${class}));
#endif // IGNORE_NEXT_PART
#undef IGNORE_NEXT_PART

namespace ${NS} {
    QScriptValue convert_script_value_f<${class}*>::operator()(QScriptEngine *eng,
                                                               ${class} * const & in) const
    {
        return in ? eng->newQObject(in) : eng->nullValue();
    }

    ${class} * convert_script_value_f<${class}*>::operator()( QScriptEngine *,
                                                              const QScriptValue & args ) const
    {
        return qobject_cast<${class}*>(args.toQObject());
    }

    struct ${jsproto}::Impl
    {
	// Add ${jsproto}-internal data here.
	Impl()
	{}
	~Impl()
	{}
    };

#define SELF(RV) ${class} *self = this->self(); \
    QScriptEngine * js = this->engine(); \
    if(!self || !js) return RV;
#define SELFQOBJ(RV) SELF(RV); \
    QObject *selfobj = qobject_cast<QObject*>(self); \
    if(!selfobj) return RV;

    ${jsproto}::${jsproto}( QObject * parent )
    : QObject(parent),
      QScriptable(),
      impl(new Impl)
    {
    }

    ${jsproto}::~${jsproto}()
    {
	delete impl;
    }

    ${class} * ${jsproto}::self()
    {
	${class} * s = qscriptvalue_cast<${class}*>(this->thisObject());
	if( ! s )
	{
	    this->context()->throwError("${jsproto}::self() got a null pointer.");
	}
	return s;
    }

EOF
if [[ x != "x${funcfile}" ]]; then
    ./createMemberFuncs.sh -c ${jsproto} < ${funcfile}
else
    cat <<EOF
    int ${jsproto}::sampleFunction()
    {
	//SELF(0);
	//return self->proxiedFunction();
    }
EOF
fi

cat <<EOF
#undef SELF
#undef SELFQOBJ
} // namespace
EOF
} > ${cpp}

echo "Output is in ${cpp} and ${hpp}"
