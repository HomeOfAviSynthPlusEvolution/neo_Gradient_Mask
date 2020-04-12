# Neo Gradient Mask

Neo Gradient Mask Copyright(C) 2020 Xinyue Lu, and previous developers

Neo Gradient Mask is the hello-world type of project for dual synth wrapper based source filter.

## Usage

```python
# AviSynth+
LoadPlugin("neo-gradient-mask.dll")
neo_gm(clip, width=640, height=480, ...)
# VapourSynth
core.neo_gm.GM(clip, width=640, height=480, ...)
```

- *width*, *height*

    Size of frame.

    Default: 640, 480.

- *depth*

    Bit depth, 8..16 integer format only.

    Default: 8.

- *color*

    Color of gradient, RGB integer color.

    Default: $99ccff.

## Compilation

```cmd
mkdir build\x86
pushd build\x86
cmake -DCMAKE_GENERATOR_PLATFORM=Win32 -D_DIR=x86 ..\..\
popd
mkdir build\x64
pushd build\x64
cmake -DCMAKE_GENERATOR_PLATFORM=x64 -D_DIR=x64 ..\..\
popd
cmake --build build\x86 --config Release
cmake --build build\x64 --config Release
```

## Compilation (GCC)

```bash
mkdir -p build/gcc
pushd build/gcc
cmake -G "MSYS Makefiles" -D_DIR=gcc ../../
popd
cmake --build build/gcc
```

## License 

MIT.
