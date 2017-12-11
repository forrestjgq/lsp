#!/bin/bash

target="all"
if [ $# -eq 1 ]; then
    target=$1
fi

echo "target = $target"
clean() 
{
    for cfile in *.c
    do
        obj=${cfile%.c}
        rm -f $obj $obj.exe
    done
}

compile() {
    if [ $# -ne 1 ]; then
        echo "arg num $# != 1"
        return 1
    fi

    obj=$1
    if ! [ -e $obj.c ]; then
        echo "$obj.c not exist"
        return 2
    fi

    gcc -Wall -I../include $obj.c ../util/dbg.c -o $obj
    if [ $? -ne 0 ]; then
        echo "$obj.c compiling failed"
        return 3
    fi

    return 0
}

t_fork() {
    ret=0
    echo ""
    echo "Start test fork.c"
    echo "======================================="
    ./fork
    ret=$?
    echo ""
    echo "fork done"

    return $ret
}

t_id() {

    ret=0
    echo ""
    echo "Start test id.c"
    echo "======================================="
    echo "Please input 0 to 9 to start, q to end"
    ./id | ./id | ./id | ./id
    ret=$?

    echo ""
    echo "id done"
    return $ret
}

t_execl() {

    ret=0
    echo ""
    echo "Start test execl.c"
    echo "======================================="
    echo "Please input 0 to 9 to start, q to end"
    ./execl /usr/bin/ls dir
    ret=$?
    echo ""
    echo "id done"
    return $ret
}


t_execle() {

    ret=0
    echo ""
    echo "Start test execle.c"
    echo "======================================="
    ./execle
    ret=$?


    echo ""
    echo "execle done"
    return $ret
}


t_execlp() {

    ret=0
    echo ""
    echo "Start test execlp.c"
    echo "======================================="
    ./execlp ls dir

    ret=$?

    echo ""
    echo "execlp done"
    return $ret
}


t_execv() {

    ret=0
    echo ""
    echo "Start test execv.c"
    echo "======================================="
    ./execv /usr/bin/ls dir -l

    ret=$?

    echo ""
    echo "execv done"
    return $ret
}


t_execvp() {

    ret=0
    echo ""
    echo "Start test execvp.c"
    echo "======================================="
    ./execvp ls dir -l
    ret=$?


    echo ""
    echo "execvp done"
    return $ret
}


t_execve() {

    ret=0
    echo ""
    echo "Start test execve.c"
    echo "======================================="
    ./execve

    ret=$?

    echo ""
    echo "execve done"
    return $ret
}


t_exec() {

    ret=0
    echo ""
    echo "Start test exec.c"
    echo "======================================="
    ./exec 0

    ret=$?
    echo ""
    echo "exec done"
    return $ret
}

t_system() {

    ret=0
    echo ""
    echo "Start test exec.c"
    echo "======================================="
    ./system
    ret=$?

    echo ""
    echo "exec done"
    return $ret
}
t_all() {
    ret=0
    for cfile in *.c
    do
        obj=${cfile%.c}
        compile $obj
        if [ $? -ne 0 ]; then
            ret=1
            break
        fi

        t_$obj
        if [ $? -ne 0 ]; then
            ret=2
            break
        fi
    done
    clean
    return $ret
}


code=0
case $target in
    clean)
        clean
        echo Cleaned
        ;;
    all)
        t_$target
        code=$?
        ;;

    *)
        compile $target
        code=$?
        if [ $code -eq 0 ]; then
            t_$target
            code=$?
        fi
        rm -f $target $target.exe
esac

echo ""
echo "========================================"
if [ $code -eq 0 ];then
    echo "Done"
else
    echo "Failed"
fi

exit $code



