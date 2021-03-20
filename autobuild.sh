#! /bin/bash

set -e
rm -rd 'pwd'/build/*
cd 'pwd'/build &&
    cmake.. &&
    make
cd ..
cp -r 'pwd'/src/include 'pwd'/lib