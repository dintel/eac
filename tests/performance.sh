#!/bin/bash

WINDOW_SIZES="32768 16384 8192 4096 2048 1024 512 256 128 64"
BLOCK_SIZES="65536 16384 8192 4096 2048 1024 512 256 128 64"
FILES=`find files/ -type f`
CSV_FILE="performance.csv"

for WS in $WINDOW_SIZES; do
    for BS in $BLOCK_SIZES; do
        TMP=($BS * 8)
        if [[ $TMP < $WS ]]; then
            continue;
        fi
        for FILE in $FILES; do
            echo $FILE LZ77 `./perf_single.sh $FILE $BS $WS` >> $CSV_FILE &
            echo $FILE EAC `./perf_single.sh $FILE $BS $WS -e` >> $CSV_FILE &
            wait
        done
    done
done
