#!/bin/bash

outdir=~/.mwg
test -d $outdir || mkdir $outdir

bindir=~/bin
test -d $bindir || mkdir $bindir

g++ modls.cpp -o $outdir/modls
cp l $bindir
cp ll $bindir
cp la $bindir
