/* HOUGH TRANSFORM */

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include "image.h"

using namespace std;

unsigned char CheckIsEdge(Image& img, int i, int j, Image& visited_label) {
  if ((i < 0) || (i >= img.get_size()) || (j < 0) || (j >= img.get_size())) {
    return 0;
  }
  if (visited_label.at(i, j, 0) == 1) {
    return 0;
  } else {
    visited_label.set(i, j, 1);

    if (img.at(i, j, 0) != 128) {
      return img.at(i, j, 0);
    } else {
      if ((CheckIsEdge(img, i + 1, j, visited_label) == 255) ||
          (CheckIsEdge(img, i + 1, j + 1, visited_label) == 255) ||
          (CheckIsEdge(img, i, j + 1, visited_label) == 255) ||
          (CheckIsEdge(img, i - 1, j + 1, visited_label) == 255) ||
          (CheckIsEdge(img, i - 1, j, visited_label) == 255) ||
          (CheckIsEdge(img, i - 1, j - 1, visited_label) == 255) ||
          (CheckIsEdge(img, i, j - 1, visited_label) == 255) ||
          (CheckIsEdge(img, i + 1, j + 1, visited_label) == 255)) {
        img.set(i, j, 255);
        return 255;
      } else {
        img.set(i, j, 0);
        return 0;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  char source_image_dir[128] = "./raw";
  char result_dir[128]       = "./result";

  if (argc == 3) {
    strncpy(source_image_dir, argv[1], sizeof(source_image_dir));
    strncpy(result_dir,       argv[2], sizeof(result_dir));
  } else {
    fprintf(stderr,
            "Source and result directories not sepcified.\n"
            "Using default values.\n");
  }

  char path[256];

  // read image sample1 into image data matrix
  int   image_size = 256;
  Image sample1(image_size, 1);
  snprintf(path, sizeof(path), "%s/sample1.raw", source_image_dir);
  sample1.ReadRaw(path);

  // (a) Perform edge detection on I_1 and output the resultant edge map as E.
  // Canny edge detection
  Image  image_E               = Image(sample1);
  double gaussian_filter[3][3] = { { 1.0 / 16, 2.0 / 16, 1.0 / 16 },
    { 2.0 / 16, 4.0 / 16, 2.0 / 16 },
    { 1.0 / 16, 2.0 / 16, 1.0 / 16 } };

  // Apply Gaussian filter
  image_E.Conv<3>(gaussian_filter);

  // Calculate gradient magnitude and orientation
  Image gradient    = Image(image_size, 1),
        orientation = Image(image_size, 1);
  for (int i = 0; i < image_size; i++) {
    for (int j = 0; j < image_size; j++) {
      int d_x = (image_E.at(i - 1, j + 1, 0) + 2 * image_E.at(i, j + 1, 0) + image_E.at(i + 1, j + 1, 0)) -
                (image_E.at(i - 1, j - 1, 0) + 2 * image_E.at(i, j - 1, 0) + image_E.at(i + 1, j - 1, 0)),
          d_y = (image_E.at(i + 1, j - 1, 0) + 2 * image_E.at(i + 1, j, 0) + image_E.at(i + 1, j + 1, 0)) -
                (image_E.at(i - 1, j - 1, 0) + 2 * image_E.at(i - 1, j, 0) + image_E.at(i - 1, j + 1, 0));
      double magnitude = sqrt((double)(d_x * d_x + d_y * d_y)) / 4.0;
      double angle     = atan2((double)d_y, (double)d_x);
      if (angle < 0) angle += M_PI;
      else if (angle > M_PI) angle -= M_PI;

      unsigned char direction = (unsigned char)(angle / (M_PI / 128));
      gradient.set(i, j, (int)magnitude);
      orientation.set(i, j, (int)direction);
    }
  }

  // Non-maximal suppression
  for (int i = 0; i < image_size; i++) {
    for (int j = 0; j < image_size; j++) {
      int value1, value2;
      unsigned char angle = orientation.at(i, j, 0);
      if (angle < 32) {
        value1 = gradient.at(i + 1, j, 0) * (32 - angle) / 32.0 +
                 gradient.at(i + 1, j + 1, 0) * angle / 32.0;
        value2 = gradient.at(i - 1, j, 0) * (32 - angle) / 32.0 +
                 gradient.at(i - 1, j - 1, 0) * angle / 32.0;
      } else if (angle < 64) {
        value1 = gradient.at(i + 1, j + 1, 0) * (64 - angle) / 32.0 +
                 gradient.at(i, j + 1, 0) * (angle - 32) / 32.0;
        value2 = gradient.at(i - 1, j - 1, 0) * (64 - angle) / 32.0 +
                 gradient.at(i, j - 1, 0) * (angle - 32) / 32.0;
      } else if (angle < 96) {
        value1 = gradient.at(i, j + 1, 0) * (96 - angle) / 32.0 +
                 gradient.at(i - 1, j + 1, 0) * (angle - 64) / 32.0;
        value2 = gradient.at(i, j - 1, 0) * (96 - angle) / 32.0 +
                 gradient.at(i + 1, j - 1, 0) * (angle - 64) / 32.0;
      } else {
        value1 = gradient.at(i - 1, j + 1, 0) * (128 - angle) / 32.0 +
                 gradient.at(i - 1, j, 0) * (angle - 96) / 32.0;
        value2 = gradient.at(i + 1, j - 1, 0) * (128 - angle) / 32.0 +
                 gradient.at(i + 1, j, 0) * (angle - 96) / 32.0;
      }
      if ((gradient.at(i, j, 0) >= value1) && (gradient.at(i, j, 0) >= value2)) {
        image_E.set(i, j, gradient.at(i, j, 0));
      } else {
        image_E.set(i, j, 0);
      }
    }
  }

  // Hysteretic thresholding
  // Histogram and cdf
  int hist[256] = { 0 };
  int cdf[256]  = { 0 };
  for (int i = 0; i < image_size; i++)
    for (int j = 0; j < image_size; j++) {
      hist[image_E.at(i, j, 0)]++;
    }
  cdf[0] = hist[0];
  for (int i = 1; i < 256; i++) {
    cdf[i] = cdf[i - 1] + hist[i];
  }

  // Set the threshold of edge
  const double kHighEdgeRatio       = 0.9;
  const int    kHighEdgePixelNumber = image_size * image_size * kHighEdgeRatio;

  unsigned char threshold_h = 0, threshold_l = 0;
  for (size_t i = 0; i < 256; i++) {
    if (threshold_h == 0) {
      if (cdf[i] >= kHighEdgePixelNumber) {
        threshold_h = i;
        threshold_l = threshold_h / 2;
      }
    }
    else break;
  }

  for (int i = 0; i < image_size; i++) {
    for (int j = 0; j < image_size; j++) {
      if (image_E.at(i, j, 0) >= threshold_h) {
        image_E.set(i, j, 255);
      } else if (image_E.at(i, j, 0) >= threshold_l) {
        image_E.set(i, j, 128);
      } else {
        image_E.set(i, j, 0);
      }
    }
  }

  // Connected component labeling
  Image visited_flag = Image(image_size, 1);
  visited_flag.clear();
  for (int i = 0; i < image_size; i++) {
    for (int j = 0; j < image_size; j++) {
      if (image_E.at(i, j, 0) == 128) {
        image_E.set(i, j, CheckIsEdge(image_E, i, j, visited_flag));
      }
    }
  }

  // Save results
  snprintf(path, sizeof(path), "%s/image_E.raw", result_dir);
  image_E.WriteRaw(path);

  // (b) Perform Hough transform on E and output the accumulator array as a new image, H_1, where the horizontal axis
  // and vertical axis represent theta and rho values, respectively.
  const int kHoughArraySize = 127;
  Image     image_H1(kHoughArraySize, 1);
  double    rho_max = sqrt((image_size + 0.5) * (image_size + 0.5) + (image_size + 0.5) * (image_size + 0.5));
  for (int i = 0; i < image_size; i++)
    for (int j = 0; j < image_size; j++) {
      if (image_E.at(i, j, 0) == 255) {
        double x = i + 0.5, y = j + 0.5;
        double phi = atan2(y, x);
        for (int n = 1; n <= kHoughArraySize; n++) {
          double theta_n = M_PI - 2 * M_PI * (kHoughArraySize - n) / (kHoughArraySize - 1);
          if ((phi - M_PI / 2 <= theta_n) && (theta_n <= phi + M_PI / 2)) {
            double rho_n = x * cos(theta_n) + y * sin(theta_n);
            int    m     = (int)round(kHoughArraySize - (rho_max - rho_n) * (kHoughArraySize - 1) / (2 * rho_max));
            image_H1.set(m, n, image_H1.at(m, n, 0) + 1);
          }
        }
      }
    }
  int max_value = 0;
  for (int i = 0; i < kHoughArraySize; i++)
    for (int j = 0; j < kHoughArraySize; j++) {
      if (image_H1.at(i, j, 0) > max_value)
        max_value = image_H1.at(i, j, 0);
    }
  for (int i = 0; i < kHoughArraySize; i++)
    for (int j = 0; j < kHoughArraySize; j++) {
      image_H1.set(i, j, (double)image_H1.at(i, j, 0) / max_value);
    }

  // Save results
  snprintf(path, sizeof(path), "%s/image_H1.raw", result_dir);
  image_H1.WriteRaw(path);

  // (c) Perform contrast adjustment on H_1 and output the result as H_2 for better visualization.
  int hist_H1[256] = { 0 };
  int cdf_H1[256]  = { 0 };
  for (int i = 0; i < kHoughArraySize; i++)
    for (int j = 0; j < kHoughArraySize; j++) {
      hist_H1[image_H1.at(i, j, 0)]++;
    }
  cdf_H1[0] = hist_H1[0];
  for (int i = 1; i < 256; i++) {
    cdf_H1[i] = cdf_H1[i - 1] + hist_H1[i];
  }
  int cdf_min = cdf_H1[0];
  for (int i = 0; i < 256; i++) {
    cdf_H1[i] = (cdf_H1[i] - cdf_min) * 255 / (kHoughArraySize * kHoughArraySize - cdf_min);
  }
  Image image_H2(kHoughArraySize, 1);
  for (int i = 0; i < kHoughArraySize; i++)
    for (int j = 0; j < kHoughArraySize; j++) {
      image_H2.set(i, j, cdf_H1[image_H1.at(i, j, 0)]);
    }

  // Save results
  snprintf(path, sizeof(path), "%s/image_H2.raw", result_dir);
  image_H2.WriteRaw(path);

  // (d) By utilizing the accumulator array, draw the top 10 and top 20 significant lines with different colors on the
  // edge map E and output the resultant images as D_1 and D_2, respectively.
  multimap<int, int> hough_array_map;
  for (int i = 0; i < kHoughArraySize; i++)
    for (int j = 0; j < kHoughArraySize; j++) {
      hough_array_map.insert(pair<int, int>(image_H1.at(i, j, 0), i * kHoughArraySize + j));
    }
  Image image_D1(image_size, 3), image_D2(image_size, 3);
  for (int i = 0; i < image_size; i++)
    for (int j = 0; j < image_size; j++) {
      image_D1.set(i, j, image_E.at(i, j, 0));
      image_D2.set(i, j, image_E.at(i, j, 0));
    }

  // Draw 10 lines on D_1
  double color1[10][3]; // Red
  for (int i = 0; i < 10; i++) {
    color1[i][0] = 1.0 - 0.08 * i;
    color1[i][1] = 0.1;
    color1[i][2] = 0.1 * i + 0.1;
  }
  for (auto it =  hough_array_map.rbegin(); distance(hough_array_map.rbegin(), it) < 10; it++) {
    int m        = it->second / kHoughArraySize;
    int n        = it->second % kHoughArraySize;
    double theta = M_PI * (1 - 2.0 * (kHoughArraySize - n) / (kHoughArraySize - 1));
    double rho   = rho_max * (1 - 2.0 * (kHoughArraySize - m) / (kHoughArraySize - 1));
    // fprintf(stderr, "%d -> %d -> m=%3d n=%3d -> rho=%f theta=%f\n", it->first, it->second, m, n, rho, theta);
    for (int i = 0; i < image_size; i++)
      for (int j = 0; j < image_size; j++) {
        double x = i + 0.5, y = j + 0.5;
        double rho_0 = x * cos(theta) + y * sin(theta);
        if (abs(rho - rho_0) <= 0.6) {
          image_D1.set(i, j, color1[distance(hough_array_map.rbegin(), it)]);
        }
      }
  }

  // Draw 20 lines on D_2
  double color2[20][3]; // Red
  for (int i = 0; i < 20; i++) {
    color2[i][0] = 1.0 - 0.04 * i;
    color2[i][1] = 0.1;
    color2[i][2] = 0.05 * i + 0.05;
  }
  for (auto it =  hough_array_map.rbegin(); distance(hough_array_map.rbegin(), it) < 20; it++) {
    int m        = it->second / kHoughArraySize;
    int n        = it->second % kHoughArraySize;
    double theta = M_PI * (1 - 2.0 * (kHoughArraySize - n) / (kHoughArraySize - 1));
    double rho   = rho_max * (1 - 2.0 * (kHoughArraySize - m) / (kHoughArraySize - 1));
    // fprintf(stderr, "%d -> %d -> m=%3d n=%3d -> rho=%f theta=%f\n", it->first, it->second, m, n, rho, theta);
    for (int i = 0; i < image_size; i++)
      for (int j = 0; j < image_size; j++) {
        double x = i + 0.5, y = j + 0.5;
        double rho_0 = x * cos(theta) + y * sin(theta);
        if (abs(rho - rho_0) <= 0.6) {
          image_D2.set(i, j, color2[distance(hough_array_map.rbegin(), it)]);
        }
      }
  }

  // Save results
  snprintf(path, sizeof(path), "%s/image_D1.raw", result_dir);
  image_D1.WriteRaw(path);
  snprintf(path, sizeof(path), "%s/image_D2.raw", result_dir);
  image_D2.WriteRaw(path);


  return 0;
}
