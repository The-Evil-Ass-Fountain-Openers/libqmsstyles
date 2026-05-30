#!/bin/bash

SU_CMD=sudo
if [[ -z "$(command -v $SU_CMD)" ]]; then
    SU_CMD=doas
    if [[ -z "$(command -v $SU_CMD)" ]]; then
        echo "Neither sudo or doas were detected on the system."
        exit
    fi
fi

BUILD_TOOL=""
if [[ "$*" == *"--build-tool"* ]]
then
    BUILD_TOOL="-DBUILD_TOOL=ON"
    echo "qmsstylestool building enabled."
else
    echo "qmsstylestool building disabled."
fi

cmake -DCMAKE_INSTALL_PREFIX=/usr $BUILD_TOOL -B build . || exit 1
cmake --build build || exit 1
$SU_CMD cmake --install build || exit 1

echo "LibQmsstyles installation script has finished."


