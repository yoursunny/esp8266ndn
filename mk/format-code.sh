#!/bin/bash
set -e
set -o pipefail
cd "$(dirname "${BASH_SOURCE[0]}")"/..

git ls-files '*.hpp' '*.cpp' '*.inc' '*.ino' | \
  xargs clang-format-8 -i -style=file
