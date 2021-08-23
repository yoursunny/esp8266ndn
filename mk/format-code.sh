#!/bin/bash
set -e
set -o pipefail
cd "$(dirname "${BASH_SOURCE[0]}")"/..

git ls-files '*.h' '*.c' '*.hpp' '*.cpp' '*.inc' '*.ino' | \
  grep -v src/vendor/ | \
  xargs clang-format-8 -i -style=file
