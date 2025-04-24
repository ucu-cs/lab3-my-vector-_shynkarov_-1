
# Lab work 3: Vector and Array
Authors (team): Volodymyr Shynkarov

## Prerequisites

Boost, CMake, gtests(release-1.12.1)

### Compilation

I've evolved, now I also use clang-tidy...

```
cmake -S . -B build \
  -DENABLE_CLANG_TIDY=ON
cmake --build build
```

where `-S .` tells to look for sourse file in `.`;

`-B build` generates build directory [`build` folder]! (I was doing this by hands)

### Usage

`cd build` -> `./StdVectorArray` or `./StdVectorArray_tests`

### Results

Many code was written, and many sad and happy moments lived. It's working! I've liked gtests, they are pretty straightforward. I've done all the needed work from the problem definition. There were a lot of moments to discuss, as for example, I've stuck on the test with the operators, especially, I thought that spaceship operator defines logic for the `==` operator, but, It was a lie.
