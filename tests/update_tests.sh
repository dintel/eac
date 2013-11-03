#!/bin/bash

FILES="1.raw 2.raw dima20k.txt dskool.txt electrop.txt goldbug.poe.txt modem.txt random128.bin random16.bin random1.bin random32.bin random64.bin random8.bin sp4rpt.txt"
HOST="dima.z.s3.amazonaws.com"

cd files

for FILE in $FILES; do
    wget http://$HOST/$FILE
done

cd ..
