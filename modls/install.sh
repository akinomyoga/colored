#!/bin/bash

share="$HOME/.mwg/share/modls"
bindir="$HOME/.mwg/bin"
test -d "$bindir" || mkdir -p "$bindir"

if ! [ -z "$(ls --help|grep -e'--time-style=STYLE')" ]; then
  supported_time_style=t
else
  supported_time_style=
fi

if test -n "$supported_time_style"; then
  a_sed="
    s/%LS%/ls --time-style=+%010s/g
    s|%MODLS%|$share/modls -t|
  "
else
  a_sed="
    s/%LS%/ls/g
    s|%MODLS%|$share/modls|
  "
fi

sed "$a_sed" l  > "$bindir/l"
sed "$a_sed" ll > "$bindir/ll"
sed "$a_sed" la > "$bindir/la"
chmod +x "$bindir/l" "$bindir/ll" "$bindir/la"
