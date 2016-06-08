#!/bin/bash

target_dir=$1

find . -not -path '*/\.*' -type f -name '*.so' | while read file
do
    cp -vu $file $target_dir
    cp -vu $(ldd $file | awk '{print $(NF-1)}' | grep /home) $target_dir
done
