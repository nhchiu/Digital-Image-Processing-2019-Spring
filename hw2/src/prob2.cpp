/* GEOMETRICAL MODIFICATION */

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "image.h"

using namespace std;

double Clamp(double x, double lower, double upper) {
  return min(upper, max(x, lower));
}

int main(int argc, char *argv[]) {
  char source_image_dir[128] = "./raw";
  char result_dir[128] = "./result";

  if (argc == 3) {
    strncpy(source_image_dir, argv[1], sizeof(source_image_dir));
    strncpy(result_dir, argv[2], sizeof(result_dir));
  } else {
    fprintf(stderr,
            "Source and result directories not sepcified.\n"
            "Using default values.\n");
  }

  char path[256];

  // read image sample4 into image data matrix
  Image sample4;
  snprintf(path, sizeof(path), "%s/sample4.raw", source_image_dir);
  sample4.ReadRaw(path);

  // (a) Perform edge crispening
  Image image_C = sample4;
  double mask[3][3] = {
      {-1.0, -1.0, -1.0}, {-1.0, 9.0, -1.0}, {-1.0, -1.0, -1.0}};
  image_C.Conv<3>(mask);

  // Save the result
  snprintf(path, sizeof(path), "%s/image_C.raw", result_dir);
  image_C.WriteRaw(path);

  // (b) Design a swirling disk warping method
  int image_size = sample4.get_size();
  Image image_D = Image(image_size);
  // Wrap to a circle
  for (int i = 0; i < image_size; i++) {
    for (int j = 0; j < image_size; j++) {
      double r =
          sqrt((i + 0.5 - image_size / 2.0) * (i + 0.5 - image_size / 2.0) +
               (j + 0.5 - image_size / 2.0) * (j + 0.5 - image_size / 2.0));
      double theta =
          atan2((j + 0.5 - image_size / 2.0), (i + 0.5 - image_size / 2.0));
      if (r > (image_size / 2.0)) {
        image_D.set(i, j, 255);
      } else {
        double u = image_size / 2.0 +
                   r * cos(theta) / max(abs(cos(theta)), abs(sin(theta))) -
                   0.5f;
        double v = image_size / 2.0 +
                   r * sin(theta) / max(abs(cos(theta)), abs(sin(theta))) -
                   0.5f;
        image_D.set(i, j, image_C.at(u, v));
      }
    }
  }
  Image image_D_swirled = Image(image_size);
  double r, theta, theta2;
  for (int i = 0; i < image_size; i++) {
    for (int j = 0; j < image_size; j++) {
      theta = atan2(j + 0.5 - (image_size / 2.0), i + 0.5 - (image_size / 2.0));
      r = sqrt((i + 0.5 - image_size / 2.0) * (i + 0.5 - image_size / 2.0) +
               (j + 0.5 - image_size / 2.0) * (j + 0.5 - image_size / 2.0));
      theta2 = (M_PI / 2) * (1 - (r / 128)) + theta;
      double u = image_size / 2.0 + r * cos(theta2) - 0.5f;
      double v = image_size / 2.0 + r * sin(theta2) - 0.5f;
      if (r > (image_size / 2.0)) {
        image_D_swirled.set(i, j, 255);
      } else {
        image_D_swirled.set(i, j, image_D.at(u, v));
      }
    }
  }
  // Save the result
  snprintf(path, sizeof(path), "%s/image_D.raw", result_dir);
  image_D_swirled.WriteRaw(path);

  return 0;
}
