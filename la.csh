#!/bin/csh

if ( $#argv == 0 ) then
    ls -la | ~/.mwg/modls
else
    ls -la "$@" | ~/.mwg/modls
endif
