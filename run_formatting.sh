#!/bin/bash
set -e

# Formats project files with clang-format
##### Usage ####
# ./run_formatting>

find src -iname *.cc | xargs clang-format -i
find include -iname *.h -not -path 'include/3rdparty/*'  | xargs clang-format -i


