#!/bin/sh

#
# This script will help you build ARDB
# Parameters are passed to configure
# 

dos2unix acinclude.m4
dos2unix configure.in
dos2unix Makefile.am
dos2unix wxARDB.spec

ACLOCAL=aclocal
AUTOMAKE=automake
AUTOCONF=autoconf
WX_CONFIG=`which wx-config`

export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig

rm -f configure

if test x$WX_CONFIG = x
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
if test -d /usr/local/share/aclocal
then
    if ($ACLOCAL -I /usr/local/share/aclocal)
    then
	echo "OK"
    else
	echo "Failed"
	exit 1
    fi
else
    if ($ACLOCAL)
    then
	echo "OK"
    else
	echo "Failed"
	exit 1
    fi
fi
#else 
#    echo "Skipping aclocal"
#fi

#if test ! -f install-sh
#then
    echo "Running automake"
    rm -fr autom4te.cache
    if ($AUTOMAKE --add-missing)
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
    if ($AUTOCONF)
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
if (./configure --enable-debug --with-wx-config=$WX_CONFIG $*)
then 
    echo "OK"
else
    echo "Failed"
    exit 1
fi

echo "Running make"
make clean
make -j 2 -l 4
