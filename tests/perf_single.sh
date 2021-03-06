#!/bin/bash

FILE=$1
BS=$2
WS=$3
EAC=$4

EXTENSION="lz77"
if [[ $EAC == "-e" ]]; then
    EXTENSION="eac"
fi

FILENAME=`basename $FILE`-$BS-$WS.$EXTENSION

OUTPUT=`/usr/bin/time -f "%e" -o $FILENAME.encode.time ../eac_encode -i $FILE -o $FILENAME -b $BS -n $WS $EAC`
/usr/bin/time -f "%e" -o $FILENAME.decode.time ../eac_decode -i $FILENAME -o $FILENAME.orig -b $BS $EAC > /dev/null
diff -q $FILENAME.orig $FILE > /dev/null
if [[ $? == 0 ]]; then
    OUTPUT="SUCCESS;$OUTPUT"
else
    OUTPUT="FAILED;$OUTPUT"
fi

ENCODE_TIME=`tail -1 $FILENAME.encode.time`
DECODE_TIME=`tail -1 $FILENAME.decode.time`
TMP=`basename $FILE`
OUTPUT="$TMP;$BS;$WS;$EXTENSION;$ENCODE_TIME;$DECODE_TIME;$OUTPUT"

rm -f $FILENAME.encode.time $FILENAME.decode.time $FILENAME $FILENAME.orig

echo $OUTPUT > results/$FILENAME.result
