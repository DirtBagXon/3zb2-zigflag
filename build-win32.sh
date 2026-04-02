#!/usr/bin/env bash
set -Eeuo pipefail

make CC=i686-w64-mingw32-gcc \
     CXX=i686-w64-mingw32-g++ \
     LD=i686-w64-mingw32-gcc \
     AR=i686-w64-mingw32-ar \
     RANLIB=i686-w64-mingw32-ranlib \
     STRIP=i686-w64-mingw32-strip \
     OSTYPE=Windows_NT \
     -j$(nproc)
mv release/game.so release/gamex86.dll

