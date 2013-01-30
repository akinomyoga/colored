#!/bin/bash

if [ $# -eq 0 ]; then
    %LS% -l | %MODLS%
else
    %LS% -l "$@" | %MODLS%
fi
