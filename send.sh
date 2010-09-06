#!/bin/bash

if [ $# -eq 0 ]; then
    echo "usage: $0 <user@host>"
    exit 1
fi

host=$1
scp mwg.modls.tbz $host:~/.mwg/mwg.modls.tbz
ssh $host "pushd ~/.mwg && tar xjvf mwg.modls.tbz && cd mwg.modls/ && ./make.sh && popd"
