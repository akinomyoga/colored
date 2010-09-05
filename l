#!/bin/bash

if [ $# -eq 0 ]; then
    ls -lB | ~/.mwg/modls
else
    ls -lB "$@" | ~/.mwg/modls
fi
