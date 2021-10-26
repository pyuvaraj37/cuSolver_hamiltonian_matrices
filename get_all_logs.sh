#!/bin/bash

echo "All log files of all matrices sizes:"
echo "---------"

for filename in log/*.log; do
    echo "$filename"
    cat "$filename"
    # for ((i=0; i<=3; i++)); do
    #     ./MyProgram.exe "$filename" "Logs/$(basename "$filename" .txt)_Log$i.txt"
    # done
    echo "---------"
done