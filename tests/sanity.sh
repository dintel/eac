#!/bin/bash

SAVEIFS=$IFS
IFS=$(echo -en "\n\b")

for FILE in `find files/ -size -40k -size +4095c -type f -not -name .gitignore -printf '%f\n'`; do
    ../eac_encode -i files/$FILE -o test.lz77 $1 > /dev/null &
    ../eac_encode -i files/$FILE -o test.eac -e $1 > /dev/null &
    wait
    ../eac_decode -i test.lz77 -o test $1 > /dev/null
    diff -q test files/$FILE > /dev/null
    if [[ $? == 0 ]]; then
        echo SUCCESS - $FILE.lz77
    else
        echo FAILED - $FILE.lz77
    fi
    ../eac_decode -e -i test.eac -o test $1 > /dev/null
    diff -q test files/$FILE > /dev/null
    if [[ $? == 0 ]]; then
        echo SUCCESS - $FILE.eac
    else
        echo FAILED - $FILE.eac
    fi
done

rm -f test.lz77 test.eac test

IFS=$SAVEIFS
