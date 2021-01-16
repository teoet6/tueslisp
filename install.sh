#!/bin/sh

./compile.sh

if [ ! -d /usr/include/tueslisp ]; then
    mkdir /usr/include/tueslisp
fi

cp src/tueslisp.h /usr/include/tueslisp.h
cp src/builtin.h /usr/include/builtin.h
