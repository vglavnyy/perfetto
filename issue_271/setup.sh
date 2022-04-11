#!/bin/bash

export CC=clang
export CXX=clang++
export CC_LD=lld
export CXX_LD=lld

base_path=$PWD/../out/
rm -rf $base_path/build.issue_271
meson setup --buildtype=debug --prefix=$base_path/out.issue_271 $base_path/build.issue_271
ninja -C $base_path/build.issue_271

