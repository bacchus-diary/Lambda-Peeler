#!/bin/bash

target_dir=$(dirname $0)

find $HOME/.local/bin/ -type f | while read file
do
    cp -vu $file $target_dir
done
