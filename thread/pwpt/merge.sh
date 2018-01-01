#!/bin/bash

echo "old pthread.md is moved to pthread.md.bk"
mv ./pthread.md ./pthread.md.bk
echo "----------------------------------------"

# 1. add <span> for each title
# 2. link for Section x.x.x to corresponding title
# 3. find <center>**FIGURE x.x** and add <span> for that
# 4. find ![**FIGURE x.x** and add <span> for that
# 5. link all [fF]igure x.x to corresponding span
cnt=0
while [ $cnt -le 8 ]
do
    echo "process pthread$cnt.md"
    sed -r '
        s/^(#+ )(([0-9]{0,2}\.)*([0-9]{1,2}))( .*)$/\<span id=\"\2\"\> \<\/span\>\n&/ 
        s/([sS]ection )([0-9.]+)/\[\*\*\1\2\**\]\(#\2\)/g
        ' < pthread$cnt.md >> pthread.md
    let "cnt=cnt+1"
done



