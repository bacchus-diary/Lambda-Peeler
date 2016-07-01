#!/bin/bash

lib_dir=$1
bin_dir=$(dirname $0)/bin

find $(dirname $0)/*/ -executable -type f ! -name '*.so' ! -name '*Specs' | while read file
do
    target=$bin_dir/$(basename $file)
    mkdir -vp $(dirname $target)
    cp -vu $file $target

    ldd $target | awk '{print $(NF-1)}' | grep /.stack | while read lib
    do
        cp -vu $lib $lib_dir
    done
done
