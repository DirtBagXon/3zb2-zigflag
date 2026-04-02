#!/usr/bin/env bash
set -Eeuo pipefail

make -j$(nproc)
mv release/game.so release/gamex86_64.so

