#!/bin/bash

HOST="dima.z.s3.amazonaws.com"
FILES="1.raw betterplace.docx betterplace.jpg betterplace.pptx car1.jpg car3.png CCITT_3.TIF CCITT_4.TIF CCITT_5.TIF charge.jpg containers.pdf dima20k.txt dskool.txt electrop.txt entrop_082-32.bin entrop_082-32.txt entrop_082-64.bin entrop_082-64.txt entrop_max-32.bin entrop_max-32.txt G31DS.TIF G31D.TIF goldbug.poe.txt modem.txt random128.bin random16.bin random1.bin random32.bin random64.bin random8.bin requirements.pdf saga1.jpg sdb-qrc.pdf sns-qrc.pdf sp4rpt.txt"

cd files

for FILE in $FILES; do
    wget http://$HOST/$FILE
done

cd ..
