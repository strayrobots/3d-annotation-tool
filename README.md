# LabelStudio

## Installation

Initialize submodules with `git submodule update --init --recursive`. Initialize git large file storage using `git lfs install`.

Create build folder `mkdir -p build && cd build`. Build and run with `cmake .. -DCMAKE_BUILD_TYPE=Release && make -j4 main && ./main`.

## Usage

Run `./main <dataset-folder>`. The dataset folder is the one containing a `scene/` subdirectory. The `scene/` subdirectory in turn should contain an `integrated.ply` mesh file created by the reconstruction system.

Keypoints are saved at `<dataset-folder>/keypoints.json` as a list of objects with attributes `x`, `y` and `z` which correspond to coordinates in the local coordinate system of the `integrated.ply` mesh.

