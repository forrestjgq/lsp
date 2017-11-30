#!/bin/bash

echo ======================
echo "Start testing mmap.c"
echo ======================
cat mmap.c > t
cat mmap.c >> t
cat mmap.c >> t
cat mmap.c >> t
cat mmap.c >> t

cat -n t > test.c

gcc mmap.c
if [ $? -eq 0 ]; then
    ./a.out test.c
    if [ $? -eq 0 ]; then
        echo ======================
        echo "Test mmap.c succeed!"
        echo ======================
    else
        echo ======================
        echo "Test mmap.c failed!"
        echo ======================
    fi
else
    echo ======================
    echo "Compile mmap.c failed"
    echo ======================
fi

rm a.out test.c t


