#!/bin/sh
set -e

VERSION=1.15.1-hypahub

sudo apt install -y fakeroot libfuse-dev libtool rman

REPO="${0%/*}/.."
cd "$REPO"
REPO="$PWD"
rm -fr build

(
    ./autogen.sh
    ./configure --prefix="$REPO/build/bindfs"
    make
    make install
)

(
    mkdir -p build/bindfs/share
    cd build/bindfs/share
    mkdir bindfs
    cd bindfs
    fakeroot "$REPO/releasescript/makerelease.sh" $VERSION
)

fakeroot tar -C build -czf bindfs-$VERSION.tar.gz bindfs

rm -r build
