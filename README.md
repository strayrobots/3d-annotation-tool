# LabelStudio

## Installation

System package dependecies are:
- glfw3
- eigen3
- OpenMP

On Ubuntu these can be installed with `sudo apt-get libeigen3-dev libglfw3-dev libomp-dev`.

Initialize submodules with `git submodule update --init --recursive`. Initialize git large file storage using `git lfs install`.

Create build folder `mkdir -p build && cd build`. Build and run with `cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4 main && ./main`.

## Usage

Run `./main <dataset-folder>`. The dataset folder is the one containing a `scene/` subdirectory. The `scene/` subdirectory in turn should contain an `integrated.ply` mesh file created by the reconstruction system.

Keypoints are saved at `<dataset-folder>/keypoints.json` as a list of objects with attributes `x`, `y` and `z` which correspond to coordinates in the local coordinate system of the `integrated.ply` mesh.


## Running tests

In your build directory, run:
```
cmake .. &&
make build_tests &&  # or ninja build_tests if using ninja.
ctest
```

## Code formatting

Code can be formatted using clang-format.

Install clang-format on Mac (brew):

`brew install clang-format`

Install clang-format on Ubuntu:

`sudo apt install clang-format`

 There's a script that runs formatting on all .h and .cc files based on the .clang-format file. Usage: `./run_formatting`

