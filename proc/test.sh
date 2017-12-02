#!/bin/bash

clean() 
{
    for cfile in *.c
    do
        obj=${cfile%.c}
        rm -f $obj $obj.exe
    done
}

for cfile in *.c
do
    obj=${cfile%.c}
    gcc $cfile -o $obj
    if [ $? -ne 0 ]; then
        echo "$cfile compiling failed"
        clean
        return 2
    fi
done

echo ""
echo "Start test id.c"
echo "======================================="
echo "Please input 0 to 9 to start, q to end"
./id | ./id | ./id | ./id

echo ""
echo "Start test execl.c"
echo "======================================="
echo "Please input 0 to 9 to start, q to end"
./execl ./id id_execl


echo ""
echo ""
echo "Done"
clean
