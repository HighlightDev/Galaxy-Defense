#!/bin/bash

echo "Binary path provided: $1"
echo "Binary name provided: $2"
echo "Binary arguments: $3"

CURRENT_DIR=`pwd`
OUTPUT_PATH="$(pwd)/output"
OUTPUT_DATA="$OUTPUT_PATH/perf.data"
mkdir $OUTPUT_PATH
cd "$1"
perf record -F 99 -a -g -o "$OUTPUT_DATA" ./"$2" "$3"
cd $CURRENT_DIR

echo "Data with callstack created, check $OUTPUT_PATH"
