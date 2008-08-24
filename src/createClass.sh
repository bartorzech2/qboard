#!/bin/bash
# A quick hack to write out some boilerplate for
# QBoard Serializable types.
if [[ "$1x" = "x" ]]; then
    echo "Usage: $0 CLASSNAME"
    exit 1
fi

cl=$1

hpp=$cl.h
cpp=$cl.cpp

for i in $hpp $cpp; do
    if [[ -e $i ]]; then
	echo "Error: file already exists: $i"
	exit 2
    fi
done

cat <<EOF > $hpp
#ifndef QBOARD_${cl}_H_INCLUDED
#define QBOARD_${cl}_H_INCLUDED 1

#include <QObject>
#include "Serializable.h"

class ${cl} : public QObject,
    public Serializable
{
Q_OBJECT
public:
    ${cl}();
    virtual ~${cl}();

    /**
       Serializes this object to dest.
    */
    virtual bool serialize( S11nNode & dest ) const;

    /**
       Deserializes this object from src.
    */
    virtual bool deserialize( S11nNode const & src );

private:
    struct Impl;
    Impl * impl;
};

#endif // QBOARD_${cl}_H_INCLUDED
EOF

cat <<EOF > $cpp
#include "${cl}.h"

struct ${cl}::Impl
{
    Impl()
    {
    }
    ~Impl()
    {
    }
};

${cl}::${cl}() : QObject(),
    Serializable("${cl}"),
    impl(new Impl)
{
}

${cl}::~${cl}()
{
    delete impl;
}

bool ${cl}::serialize( S11nNode & dest ) const
{
    if( ! this->Serializable::serialize( dest ) ) return false;
    typedef S11nNodeTraits NT;
    return true;
}

bool ${cl}::deserialize( S11nNode const & src )
{
    if( ! this->Serializable::deserialize( src ) ) return false;
    typedef S11nNodeTraits NT;
    return true;
}


EOF

echo "Output is in $cpp and $hpp"
exit 0

