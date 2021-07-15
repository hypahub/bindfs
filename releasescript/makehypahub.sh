#!/bin/sh
set -e

VERSION=1.15.1-hypahub

if type apt >/dev/null 2>&1; then
    sudo apt install -y fakeroot libfuse-dev libtool rman
    suffix=ubuntu
elif type yum >/dev/null 2>&1; then
    sudo amazon-linux-extras install epel -y
    sudo yum install -y autoconf automake fakeroot fuse-devel gcc git libtool
    suffix=al2
fi

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

fakeroot tar -C build -czf bindfs-$VERSION-$suffix.tar.gz bindfs

rm -r build
