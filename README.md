![Viewer Status](https://komarev.com/ghpvc/?username=atsahikian)
![Build Status](https://travis-ci.com/ATsahikian/pe-protector.svg?branch=master)
[![Build](https://github.com/ATsahikian/pe-protector/actions/workflows/ci.yml/badge.svg)](https://github.com/ATsahikian/pe-protector/actions/workflows/ci.yml)

# Pe-protector

Pe-protector helps you hide your sensitive windows executable files from being cracked. The stub file can be tuned to make protected file unique and undetectable.

Features:
- Mutation of assembler instructions.
- Build-in x86 assembler.
- Highly extensible and flexible stub file.
- Compression of source file.

# Build
Install dependencies:
- `choco install choco-packages.config -y`

Build steps:
- `git clone --recurse-submodules https://github.com/ATsahikian/pe-protector`
- `cd ./pe-protector`
- `cmake --preset release && cmake --build --preset release && ctest --preset release`  
