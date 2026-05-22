#!/usr/bin/env bash
set -Eeuo pipefail

make -j$(nproc)
mv release/game.so release/gamex64.so

