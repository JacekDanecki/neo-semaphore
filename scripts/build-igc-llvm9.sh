#!/bin/bash
#
# Copyright (C) 2018-2020 Intel Corporation
#
# SPDX-License-Identifier: MIT
#

set -x

IGC=($(grep -B 1  intelgraphicscompiler manifest.yml))

mkdir /root/build-igc
cd /root/build-igc


export cclang_commit_id=e604963
export spirv_id=07f2978
export igc_commit_id=${IGC[1]}

wget --no-check-certificate https://github.com/intel/opencl-clang/archive/${cclang_commit_id}/opencl-clang.tar.gz
wget --no-check-certificate https://github.com/intel/intel-graphics-compiler/archive/${igc_commit_id}/igc.tar.gz
wget --no-check-certificate https://github.com/KhronosGroup/SPIRV-LLVM-Translator/archive/${spirv_id}/spirv-llvm-translator.tar.gz

mkdir igc opencl_clang llvm-spirv

tar xzf opencl-clang.tar.gz -C opencl_clang --strip-components=1
tar xzf igc.tar.gz -C igc --strip-components=1
tar xzf spirv-llvm-translator.tar.gz -C llvm-spirv --strip-components=1

mkdir build_spirv
cd build_spirv
cmake ../llvm-spirv -DCMAKE_INSTALL_PREFIX=install -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_BUILD_TYPE=Release
make llvm-spirv -j`nproc`
make install
cd ..

mkdir build_opencl_clang
cd build_opencl_clang
cmake -DCOMMON_CLANG_LIBRARY_NAME=opencl_clang -DLLVMSPIRV_INCLUDED_IN_LLVM=OFF -DSPIRV_TRANSLATOR_DIR=../build_spirv/install \
    -DLLVM_NO_DEAD_STRIP=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX='/usr' ../opencl_clang
make -j`nproc` all
make install
cd ..

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DIGC_OPTION__OUTPUT_DIR=../igc-install/Release -DCOMMON_CLANG_LIBRARY_NAME=opencl_clang \
-DCMAKE_INSTALL_PREFIX='/usr' -Wno-dev ../igc -DIGC_PREFERRED_LLVM_VERSION=9.0.0
make -j 1
make install
cd ..

