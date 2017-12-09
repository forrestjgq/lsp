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
    for cfile in *.c
    do
        obj=${cfile%.c}
        gcc -Wall -I../include $cfile -o $obj
        if [ $? -ne 0 ]; then
            echo "$cfile compiling failed"
            clean
            exit 2
        fi
    done
}

t_fork() {

    echo ""
    echo "Start test fork.c"
    echo "======================================="
    cp fork.c test.txt
    ./fork
    head -c 120 test.txt
    echo ""
    echo "fork done"
    rm test.txt

}

t_id() {

    echo ""
    echo "Start test id.c"
    echo "======================================="
    echo "Please input 0 to 9 to start, q to end"
    ./id | ./id | ./id | ./id

    echo ""
    echo "id done"
}

t_execl() {

    echo ""
    echo "Start test execl.c"
    echo "======================================="
    echo "Please input 0 to 9 to start, q to end"
    ./execl ./id id_execl
    echo ""
    echo "id done"
}


t_execle() {

    echo ""
    echo "Start test execle.c"
    echo "======================================="
    ./execle


    echo ""
    echo "execle done"
}


t_execlp() {

    echo ""
    echo "Start test execlp.c"
    echo "======================================="
    ./execlp vim vi_bin


    echo ""
    echo "execlp done"
}


t_execv() {

    echo ""
    echo "Start test execv.c"
    echo "======================================="
    ./execv /usr/bin/vim vi_bin execv.c


    echo ""
    echo "execv done"
}


t_execvp() {

    echo ""
    echo "Start test execvp.c"
    echo "======================================="
    ./execvp vim vi_bin execvp.c


    echo ""
    echo "execvp done"
}


t_execve() {

    echo ""
    echo "Start test execve.c"
    echo "======================================="
    ./execve


    echo ""
    echo "execve done"
}


t_exec() {

    echo ""
    echo "Start test exec.c"
    echo "======================================="
    ./exec 0

    echo ""
    echo "exec done"
}


compile
case $target in
    all)
        t_fork
        t_id
        t_execle
        t_execl
        t_execlp
        t_execv
        t_execvp
        t_execve
        t_exec
        ;;

    fork)  t_fork;;
    id) t_id;;
    execle) t_execle;;
    execl) t_execl;;
    execlp) t_execlp;;
    execv) t_execv;;
    execvp) t_execvp;;
    execve) t_execve;;
    exec) t_exec;;
    *)
        echo "invalid target $target"
        exit 3
esac

echo ""
echo ""
echo "Done"

clean



