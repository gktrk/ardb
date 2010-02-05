#!/bin/sh

#
# This script will help you build ARDB
# 

# Edit the following line to reflect your wxWindows installation
WXWINDOWS_SRC_DIR=/cygdrive/c/wxWindows_2.4.0

WX_CONFIG=`which wx-config`

if test x$WX_CONFIG = x
then
    echo "error: wx-config not found in your PATH"
    echo "please check that wxWindows is correctly installed"
    exit 1
fi

if test ! -f ../sqlite/sqlite.h
then
    echo "You must build sqlite first. Do:"
    echo "    cd ../sqlite; ./configure; make"
    exit 1
fi

if test ! -f ../cards/cards.db
then 
    echo "You must build the card database first. Do:"
    echo "    cd ../cards; make"
    exit 1
fi


if test ! -f aclocal.m4 
then
    echo "Running aclocal"
    if (aclocal -I $WXWINDOWS_SRC_DIR)
    then
	echo "OK"
    else
	echo "Failed"
	exit 1
    fi
else 
    echo "Skipping aclocal"
fi

if test ! -f install-sh
then
    echo "Running automake"
    if (automake --add-missing)
    then
        echo "OK"
    else
        echo "Failed"
        exit 1
    fi
else
    echo "Skipping automake"
fi

if test ! -f configure
then
    echo "Running autoconf"
    if (autoconf)
    then
        echo "OK"
    else
        echo "Failed"
        exit 1
    fi
else
    echo "Skipping autoconf"
fi


echo "Running ./configure"
if (./configure --with-wx-config=$WX_CONFIG)
then 
    echo "OK"
else
    echo "Failed"
    exit 1
fi

echo "Running make"
make clean
make
