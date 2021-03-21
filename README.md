# LabelStudio

## Installation

Initialize submodules with `git submodule update --init --recursive`. Initialize git large file storage using `git lfs install`.

Create build folder `mkdir -p build && cd build`. Build and run with `cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4 main && ./main`.
