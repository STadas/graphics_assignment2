#!/bin/bash

cmake -S ./ -B build/ -D MAKE_STATIC_EXECUTABLE=ON
make -j4 -C build/

if [[ -f build/compile_commands.json ]]; then
    mv build/compile_commands.json ./
fi
