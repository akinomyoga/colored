#!/bin/csh

if ( $#argv == 0 ) then
    ls -lB | ~/.mwg/modls
else
    ls -lB "$@" | ~/.mwg/modls
endif

