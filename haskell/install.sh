#!/bin/bash

target_dir=$1

find . -not -path '*/\.*' -type f -name '*.so' | while read file
do
    cp -vf $file $target_dir
    cp -vf $(ldd $file | awk '{print $(NF-1)}' | grep /home) $target_dir
done
