# DIP Homework Assignment #3

Submission Date: April 10, 2019

## Student Info

- Name - 邱能賢
- ID # - R07922036
- email - r07922036@csie.ntu.edu.tw

## Running the Program

Compile and run all programs:

```bash
make
```

Compile and run individual program:

```bash
make [ bin/prob1 ][ bin/prob2 ]
```

The executable binaries will be compiled under `./bin` directory.

When running the program, the input image directory and output directory can be passed in as arguments.
For example:

```bash
./bin/prob1 [ input_image_directory ][ output_directory ]
```

If the input and output directory are not specified, the program by default will read `.raw` images from `./raw/` and output the results to `./result/`
(both directories should be at the same level as `src` and `bin`).

## Output Files

All outputs will be saved to the `[ output_directory ]`.

Images are saved as `image*.raw`.

## Tested Environment

- OS - macOS Mojave 10.14.4
- Compiler - g++
