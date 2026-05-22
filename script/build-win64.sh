#!/usr/bin/env bash
set -Eeuo pipefail

ARG="${1:-}"

make $ARG CC=x86_64-w64-mingw32-gcc \
     CXX=x86_64-w64-mingw32-g++ \
     LD=x86_64-w64-mingw32-gcc \
     AR=x86_64-w64-mingw32-ar \
     RANLIB=x86_64-w64-mingw32-ranlib \
     STRIP=x86_64-w64-mingw32-strip \
     -j$(nproc)
mv release/game.dll release/gamex64.dll

