# Contributing to StapleGL

## What to contribute

Right now the library has a satisfying set of features, however, there is still some stuff to be done, mainly:

- [ ] Add an API to `cubemap.hpp` that allows cubemap textures to be rendering targets (to build things such as specular maps).
- [ ] Improve documentations.
- [ ] Standardize error handling - Some classes make use of `std::optional`, some `assert`, some `std::terminate`, people hate all three and the solution I'd appreciate the most, `std::expected`, is one C++ standard away.
- [ ] Improve the build system - The monolithic `CMakeLists.txt` file used to build the examples is quite unreadable.
- [ ] Test the portability of examples - The examples have only ever been build on Arch Linux, testing them on current Windows and Apple systems is necessary.

## How to contribute

The project is fully open source, so you can do whatever you like with the code! if you wish to integrate your code with this fork, please follow these guidelines:

* If you are adding examples, keep dependencies to a minimum: I already had to integrate GLM and `stb_image.h` to make the Utah Teapot work, so to keep the repository slim it would be great if future showcases of techniques realized with StapleGL made use of the already available models and libraries
* If you are extending functionality, ensure that your code works by providing a small example as a use case. Also make sure that the documentation for your code is in line with the rest of the library.
* If you are fixing a bug, make sure to first open an issue here on Github so it can be tracked properly.
