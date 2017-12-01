#!/bin/bash

echo "Start test id.c"
echo "======================================="
gcc id.c -o id
if [ $? -eq 0 ]; then
    echo "Please input 0 to 9 to start"
    ./id | ./id | ./id | ./id

    if [ -e id ]; then
        rm id
    fi

    if [ -e id.exe ]; then
        rm id
    fi
fi

