/* IMAGE ENHANCEMENT */

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

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

unsigned char MirroredSample(Img image, int i, int j) {
  int m, n;
  if (i < 0) {
    m = -i;
  } else if (i < SIZE) {
    m = i;
  } else {
    m = 2 * SIZE - i - 2;
  }
  if (j < 0) {
    n = -j;
  } else if (j < SIZE) {
    n = j;
  } else {
    n = 2 * SIZE - j - 2;
  }
  return image[m][n];
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
  FILE *file = nullptr;
  char path[256];

  // read image "sample2.raw" into image data matrix
  unsigned char image[SIZE][SIZE];
  strncpy(path, source_image_dir, sizeof(path));
  strncat(path, "/sample2.raw", sizeof(path) - strlen(source_image_dir));
  if (!open_file(file, path, "rb"))
    exit(1);
  fread(image, sizeof(unsigned char), SIZE * SIZE, file);
  fclose(file);

  // (a) decrease the brightness to 1/2
  unsigned char image_D[SIZE][SIZE];
  for (size_t i = 0; i < SIZE; i++)
    for (size_t j = 0; j < SIZE; j++) {
      image_D[i][j] = image[i][j] / 2;
    }
  // write image data to "imageD.raw"
  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/imageD.raw", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  fwrite(image_D, sizeof(unsigned char), SIZE * SIZE, file);
  fclose(file);

  // (b) decrease the brightness to 1/3
  unsigned char image_E[SIZE][SIZE];
  for (size_t i = 0; i < SIZE; i++)
    for (size_t j = 0; j < SIZE; j++) {
      image_E[i][j] = image[i][j] / 3;
    }
  // write image data to "imageE.raw"
  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/imageE.raw", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  fwrite(image_E, sizeof(unsigned char), SIZE * SIZE, file);
  fclose(file);

  // (c) plot the histograms of I_2, D, E

  // I_2
  int hist_data[256] = {0};
  for (size_t i = 0; i < SIZE; i++)
    for (size_t j = 0; j < SIZE; j++) {
      hist_data[(int)image[i][j]]++;
    }

  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/hist_I2.csv", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  for (size_t i = 0; i < 256; i++)
    fprintf(file, "%zu, %d\n", i, hist_data[i]);
  fclose(file);

  // image_D
  int hist_data_D[256] = {0};
  for (size_t i = 0; i < SIZE; i++)
    for (size_t j = 0; j < SIZE; j++) {
      hist_data_D[(int)image_D[i][j]]++;
    }

  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/hist_D.csv", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  for (size_t i = 0; i < 256; i++)
    fprintf(file, "%zu, %d\n", i, hist_data_D[i]);
  fclose(file);

  // image_E
  int hist_data_E[256] = {0};
  for (size_t i = 0; i < SIZE; i++)
    for (size_t j = 0; j < SIZE; j++) {
      hist_data_E[(int)image_E[i][j]]++;
    }

  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/hist_E.csv", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  for (size_t i = 0; i < 256; i++)
    fprintf(file, "%zu, %d\n", i, hist_data_E[i]);
  fclose(file);

  // (d) global histogram equalization on D and E
  int cdf[256] = {0}, cdf_D[256] = {0}, cdf_E[256] = {0};
  cdf[0] = hist_data[0];
  cdf_D[0] = hist_data_D[0];
  cdf_E[0] = hist_data_E[0];
  for (size_t i = 1; i < 256; i++) {
    cdf[i] = cdf[i - 1] + hist_data[i];
    cdf_D[i] = cdf_D[i - 1] + hist_data_D[i];
    cdf_E[i] = cdf_E[i - 1] + hist_data_E[i];
  }
  for (size_t i = 0; i < 256; i++) {
    cdf[i] /= (SIZE * SIZE / 256);
    cdf_D[i] /= (SIZE * SIZE / 256);
    cdf_E[i] /= (SIZE * SIZE / 256);
  }
  Img image_H, image_HD, image_HE;
  for (size_t i = 0; i < SIZE; i++)
    for (size_t j = 0; j < SIZE; j++) {
      image_H[i][j] = cdf[image[i][j]];
      image_HD[i][j] = cdf_D[image_D[i][j]];
      image_HE[i][j] = cdf_E[image_E[i][j]];
    }

  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/image_H.raw", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  fwrite(image_H, sizeof(unsigned char), SIZE * SIZE, file);
  fclose(file);

  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/image_HD.raw", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  fwrite(image_HD, sizeof(unsigned char), SIZE * SIZE, file);
  fclose(file);

  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/image_HE.raw", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  fwrite(image_HE, sizeof(unsigned char), SIZE * SIZE, file);
  fclose(file);

  // Histogram of image_HD
  int hist_data_HD[256] = {0};
  for (size_t i = 0; i < SIZE; i++)
    for (size_t j = 0; j < SIZE; j++) {
      hist_data_HD[(int)image_HD[i][j]]++;
    }

  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/hist_HD.csv", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  for (size_t i = 0; i < 256; i++)
    fprintf(file, "%zu, %d\n", i, hist_data_HD[i]);
  fclose(file);

  // Histogram of image_HE
  int hist_data_HE[256] = {0};
  for (size_t i = 0; i < SIZE; i++)
    for (size_t j = 0; j < SIZE; j++) {
      hist_data_HE[(int)image_HE[i][j]]++;
    }

  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/hist_HE.csv", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  for (size_t i = 0; i < 256; i++)
    fprintf(file, "%zu, %d\n", i, hist_data_HE[i]);
  fclose(file);

  // (e) local histogram equalization on D and E
  Img image_L, image_LD, image_LE;
  const int kNeighborSize = 32;
  int local_hist[256] = {0}, local_hist_D[256] = {0}, local_hist_E[256] = {0};
  int local_cdf[256] = {0}, local_cdf_D[256] = {0}, local_cdf_E[256] = {0};
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      // Collect local histogram
      memset(local_hist, 0, 256 * sizeof(int));
      memset(local_hist_D, 0, 256 * sizeof(int));
      memset(local_hist_E, 0, 256 * sizeof(int));
      for (int k = -(kNeighborSize / 2); k < kNeighborSize / 2; k++)
        for (int l = -(kNeighborSize / 2); l < kNeighborSize / 2; l++) {
          local_hist[MirroredSample(image, i + k, j + l)]++;
          local_hist_D[MirroredSample(image_D, i + k, j + l)]++;
          local_hist_E[MirroredSample(image_E, i + k, j + l)]++;
        }
      // Local CDF
      local_cdf[0] = local_hist[0];
      local_cdf_D[0] = local_hist_D[0];
      local_cdf_E[0] = local_hist_E[0];
      for (size_t k = 1; k < 256; k++) {
        local_cdf[k] = local_cdf[k - 1] + local_hist[k];
        local_cdf_D[k] = local_cdf_D[k - 1] + local_hist_D[k];
        local_cdf_E[k] = local_cdf_E[k - 1] + local_hist_E[k];
      }
      for (size_t k = 0; k < 256; k++) {
        local_cdf[k] /= (kNeighborSize * kNeighborSize / 256);
        local_cdf_D[k] /= (kNeighborSize * kNeighborSize / 256);
        local_cdf_E[k] /= (kNeighborSize * kNeighborSize / 256);
      }
      // Histogram equalization
      image_L[i][j] = local_cdf[image[i][j]];
      image_LD[i][j] = local_cdf_D[image_D[i][j]];
      image_LE[i][j] = local_cdf_E[image_E[i][j]];
    }
  }

  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/image_L.raw", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  fwrite(image_L, sizeof(unsigned char), SIZE * SIZE, file);
  fclose(file);

  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/image_LD.raw", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  fwrite(image_LD, sizeof(unsigned char), SIZE * SIZE, file);
  fclose(file);

  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/image_LE.raw", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  fwrite(image_LE, sizeof(unsigned char), SIZE * SIZE, file);
  fclose(file);


  // Histogram of image_LD
  int hist_data_LD[256] = {0};
  for (size_t i = 0; i < SIZE; i++)
    for (size_t j = 0; j < SIZE; j++) {
      hist_data_LD[(int)image_LD[i][j]]++;
    }

  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/hist_LD.csv", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  for (size_t i = 0; i < 256; i++)
    fprintf(file, "%zu, %d\n", i, hist_data_LD[i]);
  fclose(file);

  // Histogram of image_HE
  int hist_data_LE[256] = {0};
  for (size_t i = 0; i < SIZE; i++)
    for (size_t j = 0; j < SIZE; j++) {
      hist_data_LE[(int)image_LE[i][j]]++;
    }

  strncpy(path, result_dir, sizeof(path));
  strncat(path, "/hist_LE.csv", sizeof(path) - strlen(result_dir));
  if (!open_file(file, path, "wb"))
    exit(1);
  for (size_t i = 0; i < 256; i++)
    fprintf(file, "%zu, %d\n", i, hist_data_LE[i]);
  fclose(file);

  return 0;
}
