#!/usr/bin/env bash
set -Eeuo pipefail

make CC=x86_64-w64-mingw32-gcc \
     CXX=x86_64-w64-mingw32-g++ \
     LD=x86_64-w64-mingw32-gcc \
     AR=x86_64-w64-mingw32-ar \
     RANLIB=x86_64-w64-mingw32-ranlib \
     STRIP=x86_64-w64-mingw32-strip \
     OSTYPE=Windows_NT \
     -j$(nproc)
mv release/game.so release/gamex86_64.dll

