#!/bin/bash

set -e

cmake --build build -t AbstractArtRevival
clear
./build/AbstractArtRevival "$@"
