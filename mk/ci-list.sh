#!/bin/bash
set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")"/..
CHIP=$1
FQBN=$2
BUILDARGS="$3"

echo 'set -euo pipefail'
for E in $(find ./examples -name '*.ino' -printf '%h\n'); do
  if [[ -f $E/.ci.json ]]; then
    cat $E/.ci.json
  else
    echo '{}'
  fi | jq -r --arg chip $CHIP --arg fqbn $FQBN --arg sketch $E --arg buildArgs "$BUILDARGS" '
    .[$chip] // [""] | .[] |
    [
      "printf \"\\n----\\033[1;35m Build " +
        ($sketch | sub(".*/"; "")) + " in " + $fqbn +
        (if .!="" then " with " + . else "" end) +
        " \\033[0m----\\n\"",
      "arduino-cli compile -b " + $fqbn + " --warnings more " + $sketch + " " + $buildArgs + " " + .
    ] | .[]
  '
done
