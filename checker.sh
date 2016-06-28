#!/bin/bash

set -x

whoami
env | sort

ls -la
ls -la haskell

ldd haskell/peeler

unzip abc || echo 'OK'
