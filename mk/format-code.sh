#!/bin/bash
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")"/..

git ls-files -- ':!:src/vendor' '*.h' '*.c' '*.hpp' '*.cpp' '*.inc' '*.ino' | \
  xargs clang-format-15 -i -style=file
