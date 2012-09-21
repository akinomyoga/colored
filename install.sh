#!/bin/bash

outdir="$HOME/.mwg"
test -d $outdir || mkdir $outdir

bindir="$HOME/bin"
test -d "$bindir" || mkdir "$bindir"

if ! [ -z "$(ls --help|grep -e'--time-style=STYLE')" ]; then
    supported_time_style=true
else
    supported_time_style=false
fi

#g++ -o $outdir/modls modls.cpp
if $supported_time_style; then
    filter=(sed -e "s/ls /ls --time-style='+%010s' /" -e "s|/modls|/modls -t|")
    cat l  | "${filter[@]}" > "$bindir/l"
    cat ll | "${filter[@]}" > "$bindir/ll"
    cat la | "${filter[@]}" > "$bindir/la"
else
    cp l "$bindir/l"
    cp ll "$bindir/ll"
    cp la "$bindir/la"
fi

chmod +x "$bindir/l" "$bindir/ll" "$bindir/la"