/* SIMPLE MANIPULATIONS */

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

// define image size 256*256
#define SIZE 256

int open_file(FILE *&p_file, const char *filename, const char *mode = "wb") {
  if (!(p_file = fopen(filename, mode))) {
    fprintf(stderr, "Cannot open file %s!\n", filename);
    return 0;
  } else {
    // printf("File \"%s\" opened in mode \"%s\"!\n", filename, mode);
    return 1;
  }
}

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
  FILE *file;
  char path[256];

  // read image "sample1.raw" into image data matrix
  unsigned char image[SIZE][SIZE];

  strncpy(path, source_image_dir, sizeof(path));
  strncat(path, "/sample1.raw", sizeof(path) - strlen(source_image_dir));
  if (!open_file(file, path, "rb"))
    exit(1);
  fread(image, sizeof(unsigned char), SIZE * SIZE, file);
  fclose(file);

  // (a) horizontal flipping
  unsigned char image_B[SIZE][SIZE];
  for (size_t i = 0; i < SIZE; i++)
    for (size_t j = 0; j < SIZE; j++) {
      image_B[i][j] = image[i][SIZE - j - 1];
    }
  // write image data to "imageB.raw"
  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/imageB.raw", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  fwrite(image_B, sizeof(unsigned char), SIZE * SIZE, file);
  fclose(file);

  // (b) power-law transform to enhance 𝐵
  float power = 2.0f;
  unsigned char image_C[SIZE][SIZE];
  for (size_t i = 0; i < SIZE; i++)
    for (size_t j = 0; j < SIZE; j++) {
      float intensity_f = ((float)image_B[i][j]) / 255.0f;
      float intensity_g = pow(intensity_f, power);
      image_C[i][j] = (int)(intensity_g * 255.0f);
    }
  // write image data to "imageC.raw"
  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/imageC.raw", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  fwrite(image_C, sizeof(unsigned char), SIZE * SIZE, file);
  fclose(file);

  return 0;
}
