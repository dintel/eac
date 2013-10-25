#!/bin/bash

WINDOW_SIZES="32768 16384 8192 4096 2048 1024 512 256 128 64"
BLOCK_SIZES="65536 32768 16384 8192 4096 2048 1024 512 256 128 64"
FILES=`find files/ -type f -size -10M`

rm -f results/*

for FILE in $FILES; do
    for WS in $WINDOW_SIZES; do
        for BS in $BLOCK_SIZES; do
            TMP=($BS * 8)
            if [[ $TMP < $WS ]]; then
                continue;
            fi
            ./perf_single.sh $FILE $BS $WS &
            ./perf_single.sh $FILE $BS $WS -e &
        done
    done
    wait
done

wait

RESULT_FILE=result-`date +%H-%M-%d-%m-%Y`.csv
for FILE in `find results/ -type f`; do
    cat $FILE >> $RESULT_FILE
done
