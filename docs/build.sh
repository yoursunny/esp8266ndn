#!/bin/bash
cd "$( dirname "${BASH_SOURCE[0]}" )"
if ! [[ -x ./doxygen.exe ]]; then
  if [[ "$OS" == "Windows_NT" ]]; then
    curl ftp://ftp.stack.nl/pub/users/dimitri/doxygen-1.8.14.windows.x64.bin.zip > doxygen-windows.zip
    unzip doxygen-windows.zip doxygen.exe libclang.dll
    rm doxygen-windows.zip
  elif which doxygen >/dev/null; then
    ln -s $(which doxygen) ./doxygen.exe
  else
    echo "Doxygen is unavailable" >/dev/stderr
    exit 1
  fi
fi
./doxygen.exe
