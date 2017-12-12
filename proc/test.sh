#!/bin/bash

target="all"
if [ $# -eq 1 ]; then
    target=$1
fi

echo "target = $target"

lspath="/bin/ls"
if ! [ -e $lspath ]; then
    lspath="/usr/bin/ls"
fi
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

test_single() {
    if [ $# -ne 1 ]; then
        echo "test target must be given"
        return 1
    fi
    
    target=$1
    case $target in
        fork)
            ./fork
            ret=$?
            ;;
        id)
            ./id | ./id | ./id | ./id
            ret=$?
            ;;
        execl)
            ./execl $lspath dir
            ret=$?
            ;;
        execle)
            ./execle
            ret=$?
            ;;
        execlp)
            ./execlp ls dir
            ret=$?
            ;;
        execv)
            ./execv $lspath dir -l
            ret=$?
            ;;
        execvp)
            ./execvp ls dir -l
            ret=$?
            ;;
        execve)
            ./execve
            ret=$?
            ;;
        exec)
            ./exec 0
            ret=$?
            ;;
        system)
            ./system
            ret=$?
            ;;
        session)
            ./session
            ret=$?
            ;;
        daemon|daemon1)
            ./$target
            sleep 1
            pid=`ps aux | grep ./$target | grep -v grep | awk '{print $2}'`
            echo "daemon pid $pid"
            if [ "x$pid" == "x" ]; then
                ret=1
            else
                kill -9 $pid
                pid=`ps aux | grep ./$target | grep -v grep | awk '{print $2}'`
                if [ "x$pid" != "x" ]; then
                    ret=2
                fi
            fi
            ;;
        *)
            echo "$target test not supported"
            ret=10
    esac

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

        echo ""
        echo "Start test $obj.c"
        echo "----------------------------------------"

        test_single $obj
        code=$?

        echo "----------------------------------------"
        if [ $code -eq 0 ]; then
            echo "$obj.c succeed"
        else
            echo "$obj.c failed"
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
        t_all
        code=$?
        ;;

    *)
        compile $target
        code=$?
        if [ $code -eq 0 ]; then
            echo ""
            echo "Start test $target.c"
            echo "----------------------------------------"
            test_single $target
            code=$?
            echo "----------------------------------------"
            if [ $code -eq 0 ]; then
                echo "$target.c succeed"
            else
                echo "$target.c failed"
            fi
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



