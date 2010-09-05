#!/bin/bash

if [ $# -eq 0 ]; then
    ls -l | ~/.mwg/modls
else
    ls -l "$@" | ~/.mwg/modls
fi
