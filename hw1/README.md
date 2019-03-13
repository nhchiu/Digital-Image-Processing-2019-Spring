# DIP Homework Assignment #1

Submission Date: March 13, 2019

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
make [ warmup ][ prob1 ][ prob2 ]
```

The executable binaries will be compiled under `./bin` directory.

When running the program, the input image directory and output directory can be passed in as arguments.
For example:

```bash
./bin/warmup [ input_image_directory ][ output_directory ]
```

If the input and output directory are not specified, the program by default will read `.raw` images from `./raw/` and output the results to `./result/`
(both directories should be at the same level as `src` and `bin`).

## Output Files

All outputs will be saved to the `[ output_directory ]`.

Images are saved as `image*.raw`.

Histogram data will be printed to `hist_*.csv` and can be further convert to images with Excel.

The PSNR values are printed in the `PSNR.txt` file.

## Tested Environment

- OS - macOS Mojave 10.14.3
- Compiler - g++
