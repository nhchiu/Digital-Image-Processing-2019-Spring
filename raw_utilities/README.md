# Raw2Jpeg

Some simple utilities handling `.raw` files.

- `raw2jpeg`: Convert `.raw` files to `.jpg` files.
- `raw2hist`: Generate the histograms of `.raw` files and save to `.jpg` files.

## Prerequisites

The programs require OpenCV installed.

## Running the program

Compiling

```bash
make
```

Executing

```bash
./raw2jpeg [ InputDirectory ] [ OutputDirectory ]
```

```bash
./raw2hist [ InputDirectory ] [ OutputDirectory ]
```

If the input directory is not specified, it defaults to `"./"`

The output directory must exist or the program will fail. If the output directory is not specified, it defaults to be the same as input directory.

## Tested Environment

- OS - macOS Mojave 10.14.3
- Compiler - g++
- OpenCV 4.0.1
