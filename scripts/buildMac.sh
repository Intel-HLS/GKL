#!/usr/bin/env bash

mac=$1
linux=$(hostname -A | xargs echo)
dir=$(pwd)
branch=$(git symbolic-ref --short HEAD)
native='build/classes/main/com/intel/gkl/native'

# run bash script on mac
# requires password-less ssh
ssh $mac 'bash -s' <<EOF

# run finish on EXIT
function finish {
  # copy dylib back to linux
  scp ./$native/libIntelGKL.dylib $linux:$dir/$native
  rm -rf \$work
}
trap finish EXIT

# add path to yasm
PATH=/usr/local/bin:\$PATH

# create temp directory
work=\$(mktemp -d)
cd \$work

# clone repo from linux
git clone ssh://$linux/$dir
cd GKL
git checkout $branch

# build and test
./gradlew test

# bash exits after ./gradlew completes
# remaining commands in the finish trap function
EOF
