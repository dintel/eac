#!/bin/bash

FILE=$1
BS=$2
WS=$3
EAC=$4

$OUTPUT=`../eac_encode -i $FILE -o test.comp -b $BS -w $WS $EAC`
../eac_decode -i test.comp -o test > /dev/null
diff -q test files/$FILE
if [[ $? == 0 ]]; then
    OUTPUT="$OUTPUT SUCCESS"
else
    OUTPUT="$OUTPUT FAILED"
fi

rm -f test.comp test

echo $OUTPUT
