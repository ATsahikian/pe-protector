#!/bin/bash

# TODO cmake

# vcpkg
usage() {
  echo "Usage: $0 <vcpkg-root-folder>"
}

if [[ $1 = "-h" ]] || [[ $1 = "--help" ]]; then
  usage
  exit 0
fi

VCPKG_ROOT=${1:-./vcpkg}

git clone https://github.com/Microsoft/vcpkg.git ${VCPKG_ROOT} || exit 1

${VCPKG_ROOT}/bootstrap-vcpkg.sh || exit 1

${VCPKG_ROOT}/vcpkg install pe-parse || exit 1

