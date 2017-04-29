#!/bin/bash

set -e
set -u

if [ $# -ne 1 ]; then
  echo "Usage: get-header.sh TAG"
  exit 1
fi

tag="$1"
url="https://raw.githubusercontent.com/apple/swift/$tag/tools/SourceKit/tools/sourcekitd/include/sourcekitd/sourcekitd.h"
curl --fail --location "$url" -o sourcekitd/sourcekitd.h
