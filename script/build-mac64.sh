#!/usr/bin/env bash
set -Eeuo pipefail

make -j"$(sysctl -n hw.ncpu)"
