#!/bin/bash

target_dir=$1

find $(dirname $0)/ -type f -name '*.so' | while read file
do
    patchelf --add-needed 'libHSrts_thr-ghc7.10.3.so' $file
    cp -vu $file $target_dir
    ldd $file | awk '{print $(NF-1)}' | grep /.stack | while read lib
    do
        cp -vu $lib $target_dir
    done
done
