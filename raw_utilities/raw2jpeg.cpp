#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

// Define image size 256*256
#define SIZE 256

int main(int argc, char *argv[]) {
  char input_dir[256], output_dir[256];
  if (argc < 2) {
    strncpy(input_dir, "./", sizeof(input_dir));
    strncpy(output_dir, "./", sizeof(input_dir));
  } else if (argc < 3) {
    strncpy(input_dir, argv[1], sizeof(input_dir));
    strncpy(output_dir, argv[1], sizeof(input_dir));
  } else {
    strncpy(input_dir, argv[1], sizeof(input_dir));
    strncpy(output_dir, argv[2], sizeof(input_dir));
  }
  // File pointer
  FILE *file;

  unsigned char image[SIZE][SIZE];

  DIR *input_dirp;
  struct dirent *input_dp;
  if ((input_dirp = opendir(input_dir)) != nullptr) {
    // Print all the files and directories within directory
    while ((input_dp = readdir(input_dirp)) != nullptr) {
      char *filename = input_dp->d_name;
      if (strncmp(filename + strlen(filename) - 4, ".raw", 4) != 0)
        continue;

      // Read image "input_raw" into image data matrix
      char path[256];
      strncpy(path, input_dir, sizeof(path));
      strncat(path, filename, sizeof(path) - strlen(input_dir));
      if (!(file = fopen(path, "rb"))) {
        fprintf(stderr, "Cannot open input file!\n");
        exit(1);
      }
      fread(image, sizeof(unsigned char), SIZE * SIZE, file);
      fclose(file);

      Mat img(SIZE, SIZE, CV_8UC1);

      for (size_t i = 0; i < SIZE; i++)
        for (size_t j = 0; j < SIZE; j++) {
          img.at<uchar>(i, j) = image[i][j];
        }

      // show image
      // namedWindow(filename, WindowFlags::WINDOW_AUTOSIZE);
      // imshow(filename, img);
      // waitKey(0);

      // write image data to "output_jpg"
      vector<int> quality;
      quality.push_back(IMWRITE_JPEG_QUALITY);
      quality.push_back(95);
      strncpy(path, output_dir, sizeof(path));
      strncat(path, filename, sizeof(path) - strlen(output_dir));
      strncpy(path + strlen(path) - 4, ".jpg", 4);
      if (!imwrite(path, img, quality)) {
        printf("Cannot write to file: %s\n", path);
        exit(1);
      }
    }
    closedir(input_dirp);
  } else {
    // Could not open directory
    fprintf(stderr, "Cannot open dir: %s.\n", input_dir);
    exit(1);
  }

  return 0;
}
