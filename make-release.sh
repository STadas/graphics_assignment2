#!/bin/bash
rm -rf release/ && mkdir release && mkdir release/lib

if [[ -d build ]]; then
    executable=$(find build/ | grep "^build/a.out$")
    libs=$(find build/ | grep ".so$")

    echo "$executable"
    cp $executable release/
    for lib in $libs; do
        cp "$lib" release/lib/
        echo "$lib"
    done
else
    echo "there's nothing to make release from!"
fi
