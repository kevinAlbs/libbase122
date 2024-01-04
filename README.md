# libbase122

`libbase122` is a C89 library for encoding and decoding [Base122](https://blog.kevinalbs.com/base122).

`libbase122` compiles with `-std=c89`.

This repository includes `b122`, an application for encoding and decoding Base122.

## Usage

Usage of `libbase122`:
```c
#include "base122.h"
#include <stdio.h>

int main(void) {
  unsigned char *data = (unsigned char *)"\x34\x19\x2d\x46\x63\x3c\x14";
  unsigned char decoded[16] = {0};
  base122_error_t error;
  int got;
  size_t written;

  got = base122_decode(data, 8, decoded, sizeof(decoded), &written, &error);

  if (got == -1) {
    fprintf(stderr, "Error decoding: %s\n", error.msg);
    return 1;
  }
  printf("Decoded the string: %s\n", decoded);
  return 0;
}
```

Usage of `b122`:
```
cat example.jpg | b122 > encoded.b122
cat encoded.b122 | b122 -d > decoded.jpg
```

## Using with CMake

CMake targets are exported. To use libbase122 in a CMake project:

```cmake
add_executable (app app.c)
find_package (Base122 REQUIRED)
target_link_libraries (app base122::static) # Or base122::shared
```

## Contributing

Contributions are welcome.

GitHub actions is used to compile and test each commit. See [cmake.yml](.github/workflows/cmake.yml) for the platform specific C flags.

Tests may be run locally with:

```bash
cmake -S. -Bcmake-build
cmake --build cmake-build --target test-libbase122
./cmake-build/test-libbase122
```
