#!/bin/bash

set -e
# tmp_dir="$(mktemp -d)"
tmp_dir="$(pwd)"
source_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

echo "building $source_dir in $tmp_dir"

pushd "$tmp_dir"

cmake $source_dir -GNinja -DCMAKE_BUILD_TYPE=Release
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

# rm -rf $tmp_dir

