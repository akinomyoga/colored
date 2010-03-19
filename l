#!/bin/sh

function colour {
    # -rwxr-xr-x  1 k-murase jlc   74 Nov 11 15:27 escape-regex
    while read line; do
	res=`modll "$line"`
	echo -e "$res"
    done
}

ls -lB --color --time-style='+%011s %y%m%d %H:%M' $*|modll
exit 0
