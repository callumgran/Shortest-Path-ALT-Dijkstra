#!/bin/sh

FILES="$(find . -name "*.c")"
OUT="alt-dijkstra"
BUILD_CMD="gcc -o $OUT $FILES -l pthread"

if [ "$1" = "debug" ]
then
    $BUILD_CMD -g -D DEBUG && echo "$BUILD_CMD -g -D DEBUG"
else
    $BUILD_CMD -O3 && echo "$BUILD_CMD"
fi
