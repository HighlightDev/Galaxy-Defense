#!/bin/bash

echo "Binary path provided: $1"
echo "Binary name provided: $2"
echo "Binary arguments: $3"

CURRENT_DIR=`pwd`
OUTPUT_PATH="$(pwd)/output"
OUTPUT_DATA="$OUTPUT_PATH/perf.data"
mkdir $OUTPUT_PATH
cd "$1"
perf record -F 99 -g --call-graph fp -o "$OUTPUT_DATA" ./"$2" "$3"
cd $CURRENT_DIR

perf script -i "$OUTPUT_DATA" | ./FlameGraph/stackcollapse-perf.pl > ./output/out.perf-folded
perf script -i "$OUTPUT_DATA" | ./FlameGraph/flamegraph.pl ./output/out.perf-folded > ./output/perf.svg

echo "Svg with callstack created, check $OUTPUT_PATH"

