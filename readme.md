
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

### Additional tasks

I've compared push-back, copy-ctor and iteration of the `std::vector` and `my_vector`.

Some typicall results on my machine with `size_t N = 10'000'000`:

| Operation   | std (µs) | my (µs) |
|-------------|----------|---------|
| push-back   | 9 158    | 31 422  |
| copy-ctor   | 0        | 5 297   |
| iteration   | 23 282   | 9 771   |

_push-back_ is ~3 times slower. Probably because every `push_back` still does a branch-check (is size == capacity?) and then a placement `new` of one `int`. In `libstd` they are inlined and optimized.

_copy-ctor_ for trivially-copyable types (`int`), `std::vector` copy-ctor is typically a single `memcpy` under the hood, so it completes in well under one microsecond (below our clock’s resolution).

_iteration_ this one is surprising! Looks like a measurement artificat and I cannot explain this ;/

### Results

Many code was written, and many sad and happy moments lived. It's working! I've liked gtests, they are pretty straightforward. I've done all the needed work from the problem definition. There were a lot of moments to discuss, as for example, I've stuck on the test with the operators, especially, I thought that spaceship operator defines logic for the `==` operator, but, It was a lie.
