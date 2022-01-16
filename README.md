# Unnecessary Engine
Cross platform vulkan study engine

## Compiling
### Requirements
* CMake
* Conan package manager
* Git
### Steps
1. Clone the git submodules 
   * `git submodule update --init --recursive`
2. Generate project files using cmake
   * `cd yourdir`
   * `cmake (project dir) -DCMAKE_BUILD_TYPE=Release|Debug`
3. Build 
   * `cmake --build yourdir`