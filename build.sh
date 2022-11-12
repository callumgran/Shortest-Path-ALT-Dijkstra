#!/bin/sh

FILES="$(find . -name "*.c" | xargs)"
OUT="alt-dijkstra"
PREPROCESSED_FILES_DIR="preprocessed-files"
BUILD_CMD="cc -o $OUT $FILES -l pthread -D PREPROCESSED_FILES_DIR=$PREPROCESSED_FILES_DIR"

mkdir -p "$PREPROCESSED_FILES_DIR" && echo "mkdir -p $PREPROCESSED_FILES_DIR"

if [ "$1" = "debug" ]
then
    $BUILD_CMD -g -D DEBUG && echo "$BUILD_CMD -g -D DEBUG"
else
    $BUILD_CMD -O3 && echo "$BUILD_CMD -O3"
fi
