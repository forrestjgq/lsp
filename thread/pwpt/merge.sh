#!/bin/bash

echo "old pthread.md is moved to pthread.md.bk"
mv ./pthread.md ./pthread.md.bk
echo "----------------------------------------"

cnt=0
while [ $cnt -le 8 ]
do
    echo "process pthread$cnt.md"
    cat pthread$cnt.md sed -r 's/^(#+ )(([0-9]{0,2}\.)*([0-9]{1,2}))( .*)$/\<span id=\"\2\"\> \<\/span\>\n&/' | sed -r 's/([sS]ection )([0-9.]+)/\[\1\2\]\(#\2\)/g' > pthread.md
    let "cnt=cnt+1"
done



