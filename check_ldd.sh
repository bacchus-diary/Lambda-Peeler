#!/bin/bash

set -eux

find /var/task/lib -type f -name 'libHSPeeler.so' | while read file
do
    ldd $file | grep -v /var/task
done
