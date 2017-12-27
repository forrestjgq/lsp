#!/bin/bash

cat ./pthread.md | sed -r '/^<span/ d' | sed -r 's/^(#+ )(([0-9]{0,2}\.)*([0-9]{1,2}))( .*)$/\<span id=\"\2\"\> \<\/span\>\n&/' > tmp
sed -i -r 's/([sS]ection )([0-9.]+)([^]])/\[\1\2\]\(#\2\)\3/' ./tmp
mv ./pthread.md ./pthread.md.bk
mv tmp ./pthread.md



