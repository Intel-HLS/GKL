#!/usr/bin/env bash

mac=$1
linux=$(whoami)@$(hostname | xargs echo)
dir=$(pwd)
repo_dir_name=$(basename "$dir")
native='build/classes/java/main/com/intel/gkl/native'

# run bash script on mac
# requires password-less ssh
ssh $mac 'bash -s' <<EOF

# run finish on EXIT
function finish {
  # copy dylib back to linux
  scp ./$native/*.dylib $linux:$dir/$native
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
cd $repo_dir_name

# build and test
./gradlew test

# bash exits after ./gradlew completes
# remaining commands in the finish trap function
EOF
