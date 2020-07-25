#!/bin/bash
set -e
set -o pipefail
cd "$( dirname "${BASH_SOURCE[0]}" )"

if ! [[ -x ./doxygen.exe ]]; then
  if [[ "$OS" == "Windows_NT" ]]; then
    curl -sL https://downloads.sourceforge.net/project/doxygen/rel-1.8.17/doxygen-1.8.17.windows.x64.bin.zip > doxygen-windows.zip
    unzip doxygen-windows.zip doxygen.exe libclang.dll
    rm doxygen-windows.zip
  elif which doxygen >/dev/null; then
    ln -s $(which doxygen) ./doxygen.exe
  else
    echo "Doxygen is unavailable" >/dev/stderr
    exit 1
  fi
fi
./doxygen.exe Doxyfile 2>&1 | ./filter-Doxygen-warning.awk 1>&2

if [[ -n $GTAGID ]]; then
  GTAG='<script async src="https://www.googletagmanager.com/gtag/js?id='$GTAGID'"></script><script>window.dataLayer=window.dataLayer||[];function gtag(){dataLayer.push(arguments);}gtag("js",new Date());gtag("config","'$GTAGID'");</script>'
  find html -name '*.html' | xargs sed -i "s#</head>#$GTAG</head>#"
fi
cp _redirects html/
