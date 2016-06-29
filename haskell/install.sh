#!/bin/bash

target_dir=$1

EXTRA_LIB=libHSrts_thr-ghc$(stack exec -- ghc --numeric-version).so

find $(dirname $0)/ -type f -name '*.so' | while read file
do
    name=$(basename $file)
    target=$target_dir/${name%%-*}.so
    cp -vu $file $target

    echo "Adding '$EXTRA_LIB' to $target"
    patchelf --add-needed $EXTRA_LIB $target

    ldd $target | awk '{print $(NF-1)}' | grep /.stack | while read lib
    do
        cp -vu $lib $target_dir
    done
done
