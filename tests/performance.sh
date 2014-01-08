#!/bin/bash

WINDOW_SIZES="32768 16384 8192 4096 2048 1024 512 256 128 64 32 16 8"
BLOCK_SIZES="65536 32768 16384 8192 4096 2048 1024 512 256 128 64"
FILES=`find files/ -type f`

rm -f results/*

for FILE in $FILES; do
    FILE_SIZE=`du -sb $FILE | cut -f1`
    for BS in $BLOCK_SIZES; do
        if [[ $FILE_SIZE -ge $BS ]]; then
            ./perf_single.sh $FILE $BS 0 -e &
        fi
    done
    for WS in $WINDOW_SIZES; do
        if [[ $FILE_SIZE -ge $WS ]]; then
            ./perf_single.sh $FILE 0 $WS &
        fi
    done
    wait
done

wait

RESULT_FILE=result.csv
if [[ -s $RESULT_FILE ]]; then
    TS=`stat $RESULT_FILE -c %Y`
    DATE=`date -d @$TS +%Y-%m-%d-%H-%M`
    mv $RESULT_FILE result-$DATE.csv
fi

echo "Filename;Block size;Window size;Algorithm;Encode time;Decode time;Result;File size;Compressed size;Ratio;File longest match;Average longest match;Std deviation of longest match;Block longest match" > $RESULT_FILE
for FILE in `find results/ -type f`; do
    cat $FILE >> $RESULT_FILE
done
