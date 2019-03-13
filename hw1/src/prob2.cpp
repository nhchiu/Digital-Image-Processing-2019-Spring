/* NOISE REMOVAL */

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator>

using namespace std;

// define image size 256*256
#define SIZE 256
using Img = unsigned char[SIZE][SIZE];

int open_file(FILE *&p_file, const char *filename, const char *mode = "wb") {
  if (!(p_file = fopen(filename, mode))) {
    fprintf(stderr, "Cannot open file %s!\n", filename);
    return 0;
  } else {
    // printf("File \"%s\" opened in mode \"%s\"!\n", filename, mode);
    return 1;
  }
}

void LowPassFilter(Img image) {
  unsigned char image_ext[SIZE + 2][SIZE + 2];
  for (size_t i = 0; i < SIZE + 2; i++) {
    int m;
    switch (i) {
    case 0:
      m = 1;
      break;
    case SIZE + 1:
      m = SIZE;
      break;
    default:
      m = i;
      break;
    }
    for (size_t j = 0; j < SIZE + 2; j++) {
      int n;
      switch (j) {
      case 0:
        n = 1;
        break;
      case SIZE + 1:
        n = SIZE;
        break;
      default:
        n = j;
        break;
      }
      image_ext[i][j] = image[m - 1][n - 1];
    }
  }
  int filter[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
  float filter_sum = 0.0f;
  for (size_t i = 0; i < 3; i++)
    for (size_t j = 0; j < 3; j++) {
      filter_sum += (float)filter[i][j];
    }

  for (size_t i = 0; i < SIZE; i++) {
    for (size_t j = 0; j < SIZE; j++) {
      int sum = 0;
      for (size_t k = 0; k < 3; k++)
        for (size_t l = 0; l < 3; l++) {
          sum += ((int)image_ext[i + k][j + l]) * filter[k][l];
        }
      image[i][j] = (unsigned char)((float)sum / filter_sum);
    }
  }
  return;
}

void MedianFilter(Img image) {
  unsigned char image_ext[SIZE + 2][SIZE + 2];
  for (size_t i = 0; i < SIZE + 2; i++) {
    int m;
    switch (i) {
    case 0:
      m = 1;
      break;
    case SIZE + 1:
      m = SIZE;
      break;
    default:
      m = i;
      break;
    }
    for (size_t j = 0; j < SIZE + 2; j++) {
      int n;
      switch (j) {
      case 0:
        n = 1;
        break;
      case SIZE + 1:
        n = SIZE;
        break;
      default:
        n = j;
        break;
      }
      image_ext[i][j] = image[m - 1][n - 1];
    }
  }
  for (size_t i = 0; i < SIZE; i++) {
    for (size_t j = 0; j < SIZE; j++) {
      unsigned char masked[3 * 3];
      for (size_t k = 0; k < 3; k++)
        for (size_t l = 0; l < 3; l++) {
          masked[k + 3 + l] = image_ext[i + k][j + l];
        }
      sort(begin(masked), end(masked));
      image[i][j] = masked[4];
    }
  }
  return;
}

double MSE(Img original, Img noisy) {
  double error = 0.0f;
  for (size_t i = 0; i < SIZE; i++)
    for (size_t j = 0; j < SIZE; j++) {
      error += pow((original[i][j] - noisy[i][j]), 2);
    }
  return (error / (SIZE * SIZE));
}

double PSNR(double mse) { return (10 * log10(255 * 255 / mse)); }

int main(int argc, char *argv[]) {
  char source_image_dir[128] = "./raw";
  char result_dir[128] = "./result";
  if (argc == 3) {
    strncpy(source_image_dir, argv[1], sizeof(source_image_dir));
    strncpy(result_dir, argv[2], sizeof(result_dir));
  } else {
    fprintf(stderr, "Source and result directories not sepcified.\n"
                    "Using default values.\n");
  }

  // file pointer
  FILE *file = nullptr;
  char path[256];

  // read image "sample3", "sample4", "sample5" into image data matrix
  Img image, image_a, image_b;
  strncpy(path, source_image_dir, sizeof(path));
  strncat(path, "/sample3.raw", sizeof(path) - strlen(source_image_dir));
  if (!open_file(file, path, "rb"))
    exit(1);
  fread(image, sizeof(unsigned char), SIZE * SIZE, file);
  fclose(file);
  strncpy(path, source_image_dir, sizeof(path));
  strncat(path, "/sample4.raw", sizeof(path) - strlen(source_image_dir));
  if (!open_file(file, path, "rb"))
    exit(1);
  fread(image_a, sizeof(unsigned char), SIZE * SIZE, file);
  fclose(file);
  strncpy(path, source_image_dir, sizeof(path));
  strncat(path, "/sample5.raw", sizeof(path) - strlen(source_image_dir));
  if (!open_file(file, path, "rb"))
    exit(1);
  fread(image_b, sizeof(unsigned char), SIZE * SIZE, file);
  fclose(file);

  // (a) remove noise from image a and b
  Img image_N1, image_N2;

  memcpy(image_N1, image_a, SIZE * SIZE * sizeof(unsigned char));
  LowPassFilter(image_N1);
  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/imageN1.raw", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  fwrite(image_N1, sizeof(unsigned char), SIZE * SIZE, file);
  fclose(file);

  memcpy(image_N2, image_b, SIZE * SIZE * sizeof(unsigned char));
  MedianFilter(image_N2);
  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/imageN2.raw", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  fwrite(image_N2, sizeof(unsigned char), SIZE * SIZE, file);
  fclose(file);

  // (b) compute PSNR
  double psnr_N1, psnr_N2, psnr_a, psnr_b;
  psnr_a = PSNR(MSE(image, image_a));
  psnr_b = PSNR(MSE(image, image_b));
  psnr_N1 = PSNR(MSE(image, image_N1));
  psnr_N2 = PSNR(MSE(image, image_N2));
  printf("PSNR of a: %lf\n", psnr_a);
  printf("PSNR of b: %lf\n", psnr_b);
  printf("PSNR of N1: %lf\n", psnr_N1);
  printf("PSNR of N2: %lf\n", psnr_N2);
  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/PSNR.txt", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  fprintf(file, "sample4: %lf\n", psnr_a);
  fprintf(file, "sample5: %lf\n", psnr_b);
  fprintf(file, "N1: %lf\n", psnr_N1);
  fprintf(file, "N2: %lf\n", psnr_N2);
  fclose(file);

  return 0;
}