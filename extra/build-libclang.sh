#!/usr/bin/env bash

function on_fail {
    echo $1
    exit 1
}

origin=$(pwd)
work="$origin/libclang_work"

platform=`uname`

if [ -e $work ]; then
    rm -rf $work || on_fail "Failed cleaning work folder"
fi

llvm_root="$work/llvm"
llvm_build="$llvm_root/build"

clang_root="$work/clang"

# get llvm parser
mkdir -p $llvm_root || on_fail "Failed creating work directory"

cd $llvm_root
git clone -b stable --single-branch --depth 1 http://llvm.org/git/llvm . \
    || on_fail "Unable to git clone llvm"

cd $work

# get clang parser
mkdir -p $clang_root

cd $clang_root
git clone -b "google/stable" --single-branch --depth 1 http://llvm.org/git/clang . \
    || on_fail "Unable to git clone clang"

cd $work

# build libclang
mkdir -p $llvm_build || on_fail "Unable to create build directory"
clang_out="$work/out"
mkdir -p $clang_out || on_fail "Unable to create output directory"

cd $llvm_build
cmake \
    -DCMAKE_BUILD_TYPE:STRING=RELEASE \
    -DLLVM_EXTERNAL_CLANG_SOURCE_DIR:PATH=$clang_root \
    -DLLVM_TARGETS_TO_BUILD:STRING=X86 \
    -DLLVM_INCLUDE_DOCS:BOOLEAN=FALSE \
    -DLLVM_INCLUDE_EXAMPLES:BOOLEAN=FALSE \
    -DLLVM_INCLUDE_TESTS:BOOLEAN=FALSE \
    -DCMAKE_INSTALL_PREFIX:PATH=$clang_out \
    -DCMAKE_BUILD_WITH_INSTALL_RPATH:BOOLEAN=FALSE \
    ..

cmake --build . --target libclang --config Release || on_fail "Build failed"
cmake --build . --target install || on_fail "Move output failed"

cd $origin

# package
origin_out="$origin/libclang"
origin_out_include="$origin_out/include"
origin_out_lib="$origin_out/lib"

mkdir -p $origin_out_include || on_fail "Failed creating libclang directory"
mkdir -p $origin_out_lib || on_fail "Failed creating libclang directory"

cp -rf "$clang_out/include/clang-c" $origin_out_include \
    || on_fail "Failed to copy include files to destination"

libclang_lib="libclang.a"

if [[ "$platform" == "Darwin" ]]; then
    libclang_lib="libclang.dylib"
fi

# find file
libclang_file_path=`find "$clang_out" -name "$libclang_lib"`
libclang_file_path=`echo $libclang_file_path | head -1`

cp $libclang_file_path "$origin_out_lib/$libclang_lib" \
    || on_fail "Failed to copy library files to destination"

rm -rf $work || on_fail "Failed to clean up work directory"

