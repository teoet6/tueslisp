#!/bin/sh
gcc -Wl,--dynamic-list=./exports.txt -ldl src/*.c -o tueslisp 
