/* EDGE DETECTION */

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

unsigned char CheckIsEdge(Image& img, int i, int j, Image& visited_label) {
  if ((i < 0) || (i >= img.get_size()) || (j < 0) || (j >= img.get_size())) {
    return 0;
  }
  if (visited_label.at(i, j) == 1) {
    return 0;
  } else {
    visited_label.set(i, j, 1);

    // fprintf(stderr, "%3d %3d = %3d\n", i, j, img.at(i, j));
    if (img.at(i, j) != 128) {
      return img.at(i, j);
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

  // read image sample1, sample2, and sample3 into image data matrix
  Image sample[3];
  for (int s = 0; s < 3; s++) {
    snprintf(path, sizeof(path), "%s/sample%d.raw", source_image_dir, s + 1);
    sample[s].ReadRaw(path);
  }
  double image_size = sample[0].get_size();

  // (a) Perform first order edge detection
  Image image_E1[3];
  for (int s = 0; s < 3; s++) {
    image_E1[s] = Image(image_size);

    // Calculate gradient magnitude
    for (int i = 0; i < image_size; i++) {
      for (int j = 0; j < image_size; j++) {
        int d_row        = sample[s].at(i, j) - sample[s].at(i + 1, j + 1),
            d_column     = sample[s].at(i, j + 1) - sample[s].at(i, j + 1);
        double magnitude = Clamp(
          sqrt((double)(d_row * d_row + d_column * d_column)), 0.0, 255.0);
        image_E1[s].set(i, j, (unsigned char)magnitude);
      }
    }

    // Calculate CDF
    int hist[256] = { 0 }, cdf[256] = { 0 };
    for (int i = 0; i < image_size; i++)
      for (int j = 0; j < image_size; j++) {
        hist[image_E1[s].at(i, j)]++;
      }
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) {
      cdf[i] = cdf[i - 1] + hist[i];
    }

    // Set the threshold of edge
    const float kEdgeRatio       = 0.8;
    const int   kEdgePixelNumber = image_size * image_size * kEdgeRatio;
    int threshold;
    for (size_t i = 0; i < 256; i++) {
      if (cdf[i] > kEdgePixelNumber) {
        threshold = i;
        break;
      }
    }

    // Apply the threshold
    for (int i = 0; i < image_size; i++) {
      for (int j = 0; j < image_size; j++) {
        image_E1[s].set(i, j, (image_E1[s].at(i, j) >= threshold) ? 255 : 0);
      }
    }

    // Save results
    snprintf(path, sizeof(path), "%s/E1_sample%d.raw", result_dir, s + 1);
    image_E1[s].WriteRaw(path);
  }

  // (b) Perform second order edge detection
  Image  image_E2[3];
  double gaussian_filter[3][3] = { { 1.0 / 16, 2.0 / 16, 1.0 / 16 },
    { 2.0 / 16, 4.0 / 16, 2.0 / 16 },
    { 1.0 / 16, 2.0 / 16, 1.0 / 16 } };
  double laplacian_filter[3][3] = { { -2.0 / 8, 1.0 / 8,  -2.0 / 8    },
    { 1.0 / 8,  4.0 / 8,  1.0 / 8     },
    { -2.0 / 8, 1.0 / 8,  -2.0 / 8    } };
  for (int s = 0; s < 3; s++) {
    image_E2[s] = Image(image_size);
    Image laplacian = sample[s];

    // Gaussion filter
    laplacian.Conv<3>(gaussian_filter);

    // Calculate second-order derivatives
    laplacian.Conv<3>(laplacian_filter, 127.5);

    // Calculate threshold
    unsigned char threshold_h = 130, threshold_l = 125;

    // Thresholding
    for (int i = 0; i < image_size; i++) {
      for (int j = 0; j < image_size; j++) {
        if (laplacian.at(i, j) > threshold_h) {
          laplacian.set(i, j, 255);
        } else if (laplacian.at(i, j) < threshold_l) {
          laplacian.set(i, j, 0);
        } else {
          laplacian.set(i, j, 127);
        }
      }
    }

    // Zero-crossing
    for (int i = 0; i < image_size; i++) {
      for (int j = 0; j < image_size; j++) {
        if (((laplacian.at(i, j + 1) > 127) && (laplacian.at(i, j - 1) < 127)) ||
            ((laplacian.at(i, j + 1) < 127) && (laplacian.at(i, j - 1) > 127)) ||
            ((laplacian.at(i - 1, j) > 127) && (laplacian.at(i + 1, j) < 127)) ||
            ((laplacian.at(i - 1, j) < 127) && (laplacian.at(i + 1, j) > 127)) ||
            ((laplacian.at(i + 1, j + 1) > 127) &&
             (laplacian.at(i - 1, j - 1) < 127)) ||
            ((laplacian.at(i + 1, j + 1) < 127) &&
             (laplacian.at(i - 1, j - 1) > 127)) ||
            ((laplacian.at(i - 1, j + 1) > 127) &&
             (laplacian.at(i + 1, j - 1) < 127)) ||
            ((laplacian.at(i - 1, j + 1) < 127) &&
             (laplacian.at(i + 1, j - 1) > 127))) {
          image_E2[s].set(i, j, 255);
        } else {
          image_E2[s].set(i, j, 0);
        }

        // }
      }
    }

    // Save results
    snprintf(path, sizeof(path), "%s/E2_sample%d.raw", result_dir, s + 1);
    image_E2[s].WriteRaw(path);
  }

  // (c) Perform Canny edge detection
  Image  image_E3[3];
  double gaussian_filter2[5][5] = {
    { 2.0 / 159, 4.0 / 159,  5.0 / 159,   4.0 / 159,   2.0 / 159   },
    { 4.0 / 159, 9.0 / 159,  12.0 / 159,  9.0 / 159,   4.0 / 159   },
    { 5.0 / 159, 12.0 / 159, 15.0 / 159,  12.0 / 159,  5.0 / 159   },
    { 4.0 / 159, 9.0 / 159,  12.0 / 159,  9.0 / 159,   4.0 / 159   },
    { 2.0 / 159, 4.0 / 159,  5.0 / 159,   4.0 / 159,   2.0 / 159   } };
  for (int s = 0; s < 3; s++) {
    image_E3[s] = sample[s];

    // Gaussian filter
    image_E3[s].Conv<5>(gaussian_filter2);

    // Calculate gradient magnitude and orientation
    Image gradient = Image(image_size), orientation = Image(image_size);
    for (int i = 0; i < image_size; i++) {
      for (int j = 0; j < image_size; j++) {
        int d_x = -(image_E3[s].at(i - 1, j - 1) + 2 * image_E3[s].at(i, j - 1) + image_E3[s].at(i + 1, j - 1)) +
                  (image_E3[s].at(i - 1, j + 1) + 2 * image_E3[s].at(i, j + 1) + image_E3[s].at(i + 1, j + 1)),
            d_y = -(image_E3[s].at(i - 1, j - 1) + 2 * image_E3[s].at(i - 1, j) + image_E3[s].at(i - 1, j + 1)) +
                  (image_E3[s].at(i + 1, j - 1) + 2 * image_E3[s].at(i + 1, j) + image_E3[s].at(i + 1, j + 1));
        double magnitude =
          Clamp(sqrt((double)(d_x * d_x + d_y * d_y)) / 3.0, 0.0, 255.0);
        gradient.set(i, j, (unsigned char)magnitude);
        double angle = atan2(d_y, d_x);
        if (angle < 0) angle += M_PI;
        unsigned char direction = (unsigned char)(angle / (M_PI / 128));
        orientation.set(i, j, direction);
      }
    }

    // Non-maximal suppression
    for (int i = 0; i < image_size; i++) {
      for (int j = 0; j < image_size; j++) {
        double value1, value2;
        unsigned char angle = orientation.at(i, j);
        if (angle < 32) {
          value1 = gradient.at(i + 1, j) * (32 - angle) / 32 +
                   gradient.at(i + 1, j + 1) * angle / 32;
          value2 = gradient.at(i - 1, j) * (32 - angle) / 32 +
                   gradient.at(i - 1, j - 1) * angle / 32;
        } else if (angle < 64) {
          value1 = gradient.at(i + 1, j + 1) * (64 - angle) / 32 +
                   gradient.at(i, j + 1) * (angle - 32) / 32;
          value2 = gradient.at(i - 1, j - 1) * (64 - angle) / 32 +
                   gradient.at(i, j - 1) * (angle - 32) / 32;
        } else if (angle < 96) {
          value1 = gradient.at(i, j + 1) * (96 - angle) / 32 +
                   gradient.at(i - 1, j + 1) * (angle - 64) / 32;
          value2 = gradient.at(i, j - 1) * (96 - angle) / 32 +
                   gradient.at(i + 1, j - 1) * (angle - 64) / 32;
        } else {
          value1 = gradient.at(i - 1, j + 1) * (128 - angle) / 32 +
                   gradient.at(i - 1, j) * (angle - 96) / 32;
          value2 = gradient.at(i + 1, j - 1) * (128 - angle) / 32 +
                   gradient.at(i + 1, j) * (angle - 96) / 32;
        }
        if ((gradient.at(i, j) > value1) && (gradient.at(i, j) > value2)) {
          image_E3[s].set(i, j, gradient.at(i, j));
        } else {
          image_E3[s].set(i, j, 0);
        }
      }
    }

    // Hysteretic thresholding
    // Calculate CDF
    int hist[256] = { 0 }, cdf[256] = { 0 };
    for (int i = 0; i < image_size; i++)
      for (int j = 0; j < image_size; j++) {
        hist[image_E3[s].at(i, j)]++;
      }
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) {
      cdf[i] = cdf[i - 1] + hist[i];
    }

    // Set the threshold of edge
    const double kHighEdgeRatio       = 0.9; // , kLowEdgeRatio = 0.1;
    const int    kHighEdgePixelNumber = image_size * image_size * kHighEdgeRatio;

    // kLowEdgePixelNumber = image_size * image_size * kLowEdgeRatio;
    unsigned char threshold_h = 0, threshold_l = 0;
    for (size_t i = 0; i < 256; i++) {
      if ((cdf[i] >= kHighEdgePixelNumber) && (threshold_h == 0)) {
        threshold_h = i;
        threshold_l = threshold_h / 2;
        break;
      }
    }
    for (int i = 0; i < image_size; i++) {
      for (int j = 0; j < image_size; j++) {
        if (image_E3[s].at(i, j) >= threshold_h) {
          image_E3[s].set(i, j, 255);
        } else if (image_E3[s].at(i, j) >= threshold_l) {
          image_E3[s].set(i, j, 128);
        } else {
          image_E3[s].set(i, j, 0);
        }
      }
    }

    // Connected component labeling
    Image visited_flag = Image(image_size);
    for (int i = 0; i < image_size; i++) {
      for (int j = 0; j < image_size; j++) {
        visited_flag.set(i, j, 0);
      }
    }
    for (int i = 0; i < image_size; i++) {
      for (int j = 0; j < image_size; j++) {
        if (image_E3[s].at(i, j) == 128) {
          // fprintf(stderr, "%3d, %3d: %3d\n", i, j, image_E3[s].at(i, j));
          image_E3[s].set(i, j, CheckIsEdge(image_E3[s], i, j, visited_flag));
        }
      }
    }

    // Save results
    snprintf(path, sizeof(path), "%s/E3_sample%d.raw", result_dir, s + 1);
    image_E3[s].WriteRaw(path);
  }

  return 0;
}
