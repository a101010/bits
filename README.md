# bits
A header-only modern C++ bit-masking library.

Mask and shift constants are calculated `constexpr`,  which makes the interface
a bit bizarre, but reduces runtime code to the essentials.

## Usage examples
See the comments and unit tests. You only need to add bits.hpp to your project,
the rest is only to support unit testing.

## Building the unit tests
1. Make a build directory outside the source code repository.
2. Run `cmake <path to repository>` or
   `cmake -DCMAKE_BUILD_TYPE=Debug <path to repository>`
3. If using Visual Studio, load the solution, build it, and add the
   debug target by right-clicking on the ALL_BUILD target. Enter the
   target by going to Properties->Debugging->Command, and browse to
   `tests\Debug\run_tests.exe`.
4. If using g++ or clang, just run `make`.
