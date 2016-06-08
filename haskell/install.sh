#!/bin/bash

target_dir=$1

find . -not -path '*/\.*' -type f -name '*.so' | while read file
do
    cp -vu $file $target_dir
    ldd $file | awk '{print $(NF-1)}' | grep /.stack | while read lib
    do
        cp -vu $lib $target_dir
    done
done
