#!/bin/csh

if ( $#argv == 0 ) then
    ls -l | ~/.mwg/modls
else
    ls -l "$@" | ~/.mwg/modls
endif
