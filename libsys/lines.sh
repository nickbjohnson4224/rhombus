#!/bin/sh

cat src/*/*.c | grep -v "#" | grep -v "$/" | grep -c " " | sed "s/$/ lines of C code/"
cat src/*/*.h | grep -v "#" | grep -v "$/" | grep -c " " | sed "s/$/ lines of headers/"
cat src/*/*.s | grep -v "^;" | grep -c " " | sed "s/$/ lines of NASM code/"
