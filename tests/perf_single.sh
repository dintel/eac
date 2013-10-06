#!/bin/bash

FILE=$1
BS=$2
WS=$3
EAC=$4

$OUTPUT=`/usr/bin/time -f "%e" -o encode.time ../eac_encode -i $FILE -o test.comp -b $BS -w $WS $EAC`
/usr/bin/time -f "%e" -o decode.time ../eac_decode -i test.comp -o test > /dev/null
diff -q test files/$FILE
if [[ $? == 0 ]]; then
    OUTPUT="$OUTPUT SUCCESS"
else
    OUTPUT="$OUTPUT FAILED"
fi

ENCODE_TIME=`cat encode.time`
DECODE_TIME=`cat decode.time`
OUTPUT="$ENCODE_TIME $DECODE_TIME $OUTPUT"

rm -f test.comp test test.log encode.time decode.time

echo $OUTPUT
