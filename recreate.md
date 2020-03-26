### Recreating this build
- cloned `libigl-example-project`
- added following files to root dir:
  - `menu_extra.h`
  - `slicing_plugin.h`
  - dir `imguizmo`
- modified:
  - `main.cpp`
  - `CMakeLists.txt`
  - `cmake/FindLIBIGL.cmake`
- checked out libigl/dev, branched off there
- in `libigl-example-project`:
  - `mkdir build`
  - `cd build && cmake ../`
  - `make`

### Bugs
- vertex labels
- face labels
- widget not rendering properly with orthographic camera