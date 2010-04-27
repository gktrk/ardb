#!/bin/sh

#
# This script will help you build ARDB
# 

WX_CONFIG="/usr/local/bin/wx-config"
#WX_CONFIG=`which wx-config`
export PKG_CONFIG_PATH=/opt/local/lib/pkgconfig:/usr/local/lib/pkgconfig/

if test "x$WX_CONFIG" = "x"
then
    echo "error: wx-config not found in your PATH"
    echo "please check that wxWindows is correctly installed"
    exit 1
fi

# if test ! -f ../sqlite/.libs/libsqlite.la
# then
#     echo "You must build sqlite first. Do:"
#     echo "    cd ../sqlite; ./configure; make"
#     exit 1
# fi

#if test ! -f aclocal.m4 
#then

echo "Running aclocal"
ACLOCAL_INCLUDES=""
if test -d /usr/local/share/aclocal
then
    ACLOCAL_INCLUDES="$ACLOCAL_INCLUDES -I /usr/local/share/aclocal"
fi
if test -d /sw/share/aclocal
then
    ACLOCAL_INCLUDES="$ACLOCAL_INCLUDES -I /sw/share/aclocal"
fi
if test -d /opt/local/share/aclocal
then
    ACLOCAL_INCLUDES="$ACLOCAL_INCLUDES -I /opt/local/share/aclocal"
fi

if aclocal $ACLOCAL_INCLUDES
then
    echo "OK"
else
    echo "Failed"
    exit 1
fi
#else 
#    echo "Skipping aclocal"
#fi

#if test ! -f install-sh
#then
    echo "Running automake"
    rm -fr autom4te.cache
    if automake --add-missing
    then
        echo "OK"
    else
        echo "Failed"
        exit 1
    fi
#else
#    echo "Skipping automake"
#fi

#if test ! -f configure
#then
    echo "Running autoconf"
    if autoconf
    then
        echo "OK"
    else
        echo "Failed"
        exit 1
    fi
#else
#    echo "Skipping autoconf"
#fi

if test -f config.cache
then
    rm -f config.cache
fi

echo "Running ./configure"
if ./configure --enable-debug --with-libxml2-config=/opt/local/bin/xml2-config --with-wx-config=$WX_CONFIG --with-sqlite-prefix=/usr/local
then 
    echo "OK"
else
    echo "Failed"
    exit 1
fi

echo "Running make"
make clean
make -j 4 -l 4
rm -rf ardb.app
mkdir ardb.app
mkdir ardb.app/Contents
mkdir ardb.app/Contents/MacOS
mkdir ardb.app/Contents/Resources
mkdir ardb.app/Contents/Resources/xsl
cp src/Info.plist ardb.app/Contents
cp src/ardb ardb.app/Contents/MacOS
ls -1 resources | grep -v ".xsl" | xargs -I FILE cp -r resources/FILE ardb.app/Contents/Resources
cp resources/*.xsl ardb.app/Contents/Resources/xsl

