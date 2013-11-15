#!/bin/bash

HOST="dima.z.s3.amazonaws.com"
FILES="2.raw dima20k.txt dskool.txt electrop.txt entrop_082-32.txt entrop_082-64.txt entrop_max-32.txt goldbug.poe.txt modem.txt random128.bin random16.bin random1.bin random32.bin random64.bin random8.bin sp4rpt.txt"

cd files

for FILE in $FILES; do
    wget http://$HOST/$FILE
done

cd ..
