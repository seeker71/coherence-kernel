# Local speech ear for Apple silicon

`whisper-cli` is a static Apple-silicon build of
[whisper.cpp v1.9.1](https://github.com/ggml-org/whisper.cpp/releases/tag/v1.9.1).
It is carried here so opening Sema Sessions on a new Mac does not ask a guest
to install a compiler, Homebrew, or a command-line package.

The app verifies this file before it listens:

```
c273da13b492d3f4138f3967d0c38bafb3a54e4362a6afe51145b6e22ef609c5  whisper-cli
```

It was built from the v1.9.1 source archive whose SHA-256 is
`147267177eef7b22ec3d2476dd514d1b12e160e176230b740e3d1bd600118447`:

```
cmake -S . -B build-portable \
  -DBUILD_SHARED_LIBS=OFF \
  -DWHISPER_BUILD_TESTS=OFF \
  -DWHISPER_BUILD_SERVER=OFF \
  -DGGML_METAL=OFF \
  -DGGML_OPENMP=OFF \
  -DGGML_ACCELERATE=ON \
  -DGGML_NATIVE=OFF \
  -DGGML_CPU_ARM_ARCH=armv8-a \
  -DCMAKE_OSX_ARCHITECTURES=arm64 \
  -DCMAKE_OSX_DEPLOYMENT_TARGET=13.3
cmake --build build-portable --config Release -j
```

Observed on 2026-08-17: Mach-O arm64; ad-hoc signed; dynamic links only to
macOS `libSystem`, `libc++`, and the Accelerate framework. With the repository's
audio fixture and a local Whisper model it returned `book.` with exit 0.

License: MIT; see `LICENSE.whisper.cpp`.
