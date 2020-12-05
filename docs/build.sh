#!/bin/bash
set -e
set -o pipefail
cd "$( dirname "${BASH_SOURCE[0]}" )"

if ! [[ -x ./doxygen.exe ]]; then
  rm -f ./doxygen.exe
  if [[ "$OS" == "Windows_NT" ]]; then
    curl -sL https://downloads.sourceforge.net/project/doxygen/rel-1.8.17/doxygen-1.8.17.windows.x64.bin.zip > doxygen-windows.zip
    unzip -o doxygen-windows.zip doxygen.exe libclang.dll
    rm doxygen-windows.zip
  elif which doxygen >/dev/null; then
    ln -s $(which doxygen) ./doxygen.exe
  else
    echo "Doxygen is unavailable" >/dev/stderr
    exit 1
  fi
fi
./doxygen.exe Doxyfile 2>&1 | ./filter-Doxygen-warning.awk 1>&2

find html -name '*.html' | xargs sed -i '/<\/head>/ i\ <script>(function(e,t,n,i,s,a,c){e[n]=e[n]||function(){(e[n].q=e[n].q||[]).push(arguments)};a=t.createElement(i);c=t.getElementsByTagName(i)[0];a.async=true;a.src=s;c.parentNode.insertBefore(a,c)})(window,document,"galite","script","https://cdn.jsdelivr.net/npm/ga-lite@2/dist/ga-lite.min.js"); if (location.hostname.endsWith(".ndn.today")) { galite("create", "UA-935676-11", "auto"); galite("send", "pageview"); }</script>'
cp _redirects html/
