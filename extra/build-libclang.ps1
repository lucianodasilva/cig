
function on_fail {
    param(
        $error_message
    )

    if (-not $?) {
        Write-Error $message
        exit 1
    }
}

$origin = $PSScriptRoot
$work="$origin/libclang_work"

if (Test-Path $work) {
    Remove-Item $work -Force -Recurse
    on_fail "Failed cleaning work folder"
}

$llvm_root="$work/llvm"
$llvm_build="$llvm_root/build"

$clang_root="$work/clang"

# get llvm parser
New-Item -ItemType "directory" -Path $llvm_root -Force
on_fail "Failed creating work directory"

Set-Location $llvm_root
git clone -b stable --single-branch --depth 1 http://llvm.org/git/llvm .
on_fail "Unable to git clone llvm"

Set-Location $work

# get clang parser
New-Item -ItemType "directory" -Path $clang_root -Force

Set-Location $clang_root
git clone -b "google/stable" --single-branch --depth 1 http://llvm.org/git/clang .
on_fail "Unable to git clone clang"

Set-Location $work

# build libclang
New-Item -ItemType "directory" -Path $llvm_build -Force
on_fail "Unable to create build directory"

$clang_out="$work/out"
New-Item -ItemType "directory" -Path $clang_out -Force
on_fail "Unable to create output directory"

Set-Location $llvm_build

cmake `
    -DCMAKE_BUILD_TYPE:STRING=RELEASE `
    -DLLVM_EXTERNAL_CLANG_SOURCE_DIR:PATH=$clang_root `
    -DLLVM_TARGETS_TO_BUILD:STRING=X86 `
    -DLLVM_INCLUDE_DOCS:BOOLEAN=FALSE `
    -DLLVM_INCLUDE_EXAMPLES:BOOLEAN=FALSE `
    -DLLVM_INCLUDE_TESTS:BOOLEAN=FALSE `
    -DCMAKE_INSTALL_PREFIX:PATH=$clang_out `
    -DCMAKE_BUILD_WITH_INSTALL_RPATH:BOOLEAN=FALSE `
    ..

cmake --build . --target libclang --config Release
on_fail "Build failed"

cmake --build . --target install
on_fail "Move output failed"

Set-Location $origin

# package
$origin_out="$origin/libclang"
$origin_out_include="$origin_out/include"
$origin_out_lib="$origin_out/lib"
$origin_out_bin="$origin_out/bin"

New-Item -ItemType "directory" -Path $origin_out_include -Force
on_fail "Failed creating libclang directory"

New-Item -ItemType "directory" -Path $origin_out_lib -Force
on_fail "Failed creating libclang directory"

New-Item -ItemType "directory" -Path $origin_out_bin -Force
on_fail "Failed creating libclang directory"

Copy-Item "$clang_out/include/clang-c" $origin_out_include -Recurse -Force
on_fail "Failed to copy include files to destination"

libclang_lib="libclang.lib"
libclang_dll="libclang.dll"

# copy libraries
Copy-Item "$clang_out/lib/$libclang_lib" "$origin_out_lib/$libclang_lib"
on_fail "Failed to copy library files to destination"

Copy-Item "$clang_out/bin/$libclang_bin" "$origin_out_bin/$libclang_dll"
on_fail "Failed to copy library files to destination"

Remote-Item $work -Recurse -Force
on_fail "Failed to clean up work directory"