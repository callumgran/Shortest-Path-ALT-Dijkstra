#!/bin/sh

FILES="$(find . -name "*.c")"
OUT="alt-dijkstra"
BUILD_CMD="gcc -o $OUT $FILES"

if [ "$1" = "debug" ]
then
    $BUILD_CMD -g -D DEBUG
else
    $BUILD_CMD -O3
fi
