#!/bin/bash

set -e

delete_build_dir=0
# Create a temporary directory if the script is not run from the build or project folder with an existing build folder.
# Saves time to not have to build from scratch.
if [ -f "$(pwd)/CMakeCache.txt" ]; then
  tmp_dir=$(pwd)
elif [ -f "$(pwd)/build/CMakeCache.txt" ]; then
  tmp_dir="$(pwd)/build"
else
  tmp_dir="$(mktemp -d)"
  delete_build_dir=1
fi
source_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

echo "building $source_dir in $tmp_dir"

pushd "$tmp_dir"

cmake $source_dir -GNinja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=1
ninja
cpack -GZIP
archive_name="$(find Studio-*.tar.gz)"

project_name="$(echo $archive_name | cut -d"-" -f1 | tr '[:upper:]' '[:lower:]')"
version="$(echo $archive_name | cut -d"-" -f2 | tr '[:upper:]' '[:lower:]')"
platform="$(echo $archive_name | cut -d"-" -f3 | cut -d"." -f1 | tr '[:upper:]' '[:lower:]')"
arch="$(uname -m)"
echo "Project name: $project_name"
echo "version: $version"
echo "platform: $platform"
echo "architecture: $arch"

destination="s3://stray-builds/studio/$platform/$arch/latest/studio.tar.gz"
echo "Uploading to $destination"

s3cmd put $archive_name $destination
s3cmd setacl $destination --acl-public

popd

if [ $delete_build_dir = 1 ]; then
  rm -rf $tmp_dir
fi

