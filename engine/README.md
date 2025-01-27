# Go Engine

This is a C++ implementation of a Go game engine. It supports basic Go rules, including stone placement, capturing, and the Ko rule. The engine is designed to be used with unit tests and can be extended for further functionality.

---

## Table of Contents

1. [Requirements](#requirements)
2. [Build Instructions](#build-instructions)
3. [Running Tests](#running-tests)
4. [Usage](#usage)
5. [Contributing](#contributing)
6. [License](#license)

---

## Requirements

To build and run this project, you need the following:

- **C++ Compiler**: `g++` (GNU Compiler Collection) or `clang++`.
- **CMake**: Version 3.10 or higher.
- **Google Test**: For running unit tests.
- **Make**: For building the project.

---

## Build Instructions

```bash
git clone https://github.com/realtaraharris/gogogo.git
```

## Running Tests

```bash
cd gogogo/engine
mkdir build
cd build
cmake ..
cmake --build .
ctest -V
```