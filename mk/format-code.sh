#!/bin/bash
cd "$( dirname "${BASH_SOURCE[0]}" )"/..
find -name '*.[hc]pp' -or -name '*.ino' | \
  xargs clang-format-6.0 -i -style=file
