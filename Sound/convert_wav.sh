#!/bin/bash
#
# convert e.g. stereo wav file to mono 11.025kHz 8 bit
#
if [ "$#" -ne "2" ]; then
    echo "Usage: $0 input.wav output.wav"
    exit 1
fi
echo "Convert to 11.025kHz mono"
/usr/bin/sox $1 -r 11025 -b 8 -c 1 $2 remix 1,2
