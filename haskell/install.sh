#!/bin/bash -eux

cd $(dirname $0)

lib_dir=$1
bin_dir=bin

alias stack='stack --allow-different-user'

stack --install-ghc test
stack install

find $(stack path --local-bin) -executable -type f | while read file
do
    target=$bin_dir/$(basename $file)
    mkdir -vp $(dirname $target)
    cp -vu $file $target

    ldd $target | awk '{print $(NF-1)}' | grep /.stack | while read lib
    do
        cp -vu $lib $lib_dir
    done
done
