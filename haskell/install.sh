#!/bin/bash -eux

cd $(dirname $0)

lib_dir=$1

stack='stack --allow-different-user'

$stack --install-ghc test
$stack install

EXTRA_LIB=libHSrts_thr-ghc$($stack exec -- ghc --numeric-version).so

find $($stack path --local-install-root)/lib -type f -name '*.so' | while read file
do
    set +x
    name=$(basename $file)
    target=$lib_dir/${name%%-*}.so
    cp -vu $file $target

    echo "Adding '$EXTRA_LIB' to $target"
    patchelf --add-needed $EXTRA_LIB $target

    ldd $target | awk '{print $(NF-1)}' | grep -v /var/task | while read lib
    do
        [ -f $lib ] && cp -vu $lib $lib_dir || echo "Skip '$lib'"
    done
done
