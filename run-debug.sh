#!/bin/bash

set -e

clear
cmake --build build-debug -t AbstractArtRevival
clear
gdb -ex run --args ./build-debug/AbstractArtRevival "$@"
