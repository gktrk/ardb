#!/bin/sh

#
# This script will help you build a Windows .exe from your Linux box
# Welcome to cross-compiling.
# 

#
# Edit the following values to reflect your cross-compiling installation
# See doc/cross-compiling for help on this
#
CROSS_COMPILER_PREFIX=i586-mingw32msvc
CROSS_COMPILER_PATH=/usr/local/src/minGW
CROSS_COMPILER_BIN_DIR=$CROSS_COMPILER_PATH/bin:$CROSS_COMPILER_PATH/$CROSS_COMPILER_PREFIX/bin
WXWINDOWS_SRC_DIR=/usr/local/src/wxWidgets
WXWINDOWS_BUILD_DIR=/usr/local/src/wxWidgets/build_fg/win32
LIBXML_SRC_DIR=/usr/local/src/gnome-xml
LIBXML_BUILD_DIR=/usr/local/src/gnome-xml/static-release
SQLITE_PATH=$CROSS_COMPILER_PATH/$CROSS_COMPILER_PREFIX

export PATH=$CROSS_COMPILER_BIN_DIR:$PATH
#export CXXFLAGS="-I$WXWINDOWS_BUILD_DIR/lib/wx/include/msw-2.4-$CROSS_COMPILER_PREFIX -I$WXWINDOWS_BUILD_DIR/include -I$WXWINDOWS_SRC_DIR/include "
#export LDFLAGS="-L$WXWINDOWS_BUILD_DIR/lib -L$LIBXML_BUILD_DIR"

if test ! -f $SQLITE_PATH/lib/libsqlite.a
then
    echo "No libsqlite.a found in $SQLITE_PATH/lib"
    echo "Please make sure libsqlite is installed there or edit this script"
    exit 1
fi

#if test ! -f aclocal.m4 
#then
    echo "Running aclocal"
    if (aclocal -I /usr/local/share/aclocal)
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
    if (automake --add-missing)
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
    if (autoconf)
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

CONFIGURE_COMMAND="./configure --target=$CROSS_COMPILER_PREFIX --host=$CROSS_COMPILER_PREFIX --with-wx-config=$CROSS_COMPILER_PATH/bin/wx-config --with-wx-prefix=$WXWINDOWS_SRC_DIR --with-libxml2-config=$CROSS_COMPILER_PATH/bin/xml2-config --with-wx-exec-prefix=$WXWINDOWS_BUILD_DIR --with-sqlite-prefix=$SQLITE_PATH"
echo "Running $CONFIGURE_COMMAND"
if ($CONFIGURE_COMMAND)
then
    echo "OK"
else
    echo "Failed"
    exit 1
fi

echo "Running make"
make clean
make -j 2 -l 4

i586-mingw32msvc-strip src/ardb.exe
