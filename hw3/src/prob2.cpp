/* TEXTURE ANALYSIS */

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "image.h"
#include "morphological_processing.h"
#include "texture_analysis.h"

using namespace std;

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

  // read image sample2 into image data matrix
  int   image_size = 512;
  Image sample2(image_size, 1);
  snprintf(path, sizeof(path), "%s/sample2.raw", source_image_dir);
  sample2.ReadRaw(path);

  // (a) Perform Law’s method on I_2
  Texture sample2_texture;
  sample2_texture.Analyze(sample2);

  // (b) Use k-means to classify each pixel and label same kind of texture with same gray-level intensity
  int k             = 4;
  int gray_level[4] = { 64, 128, 192, 255 };
  double k_centroid[4][9];
  for (int i = 0; i < k; i++)
    for (int j = 0; j < 9; j++) {
      k_centroid[i][j] = sample2_texture.get_features(j).at(85 * i, 85 * i, 0);
    }
  Image image_E(image_size, 1);
  bool  should_break = false;
  while (!should_break) {
    // Nearest neighbor clustering
    for (int i = 0; i < image_size; i++) {
      for (int j = 0; j < image_size; j++) {
        double min_distance = 100000;
        int    cluster      = 0;
        for (int f = 0; f < k; f++) {
          double distance = 0;
          for (int l = 0; l < 9; l++) {
            double x = (double)sample2_texture.get_features(l).at(i, j, 0);
            distance += (x - k_centroid[f][l]) * (x - k_centroid[f][l]);
          }
          distance = sqrt(distance / 9);
          if (distance <= min_distance) {
            min_distance = distance;
            cluster      = f;
          }
        }
        image_E.set(i, j, 0, cluster);
      }
    }

    // Re-compute the centroid
    double centroid_sum[4][9] = { { 0 } };
    int    centroid_count[4]  = { 0 };
    for (int i = 0; i < image_size; i++)
      for (int j = 0; j < image_size; j++) {
        int cluster = image_E.at(i, j, 0);
        centroid_count[cluster]++;
        for (int l = 0; l < 9; l++)
          centroid_sum[cluster][l] += (double)sample2_texture.get_features(l).at(i, j, 0);
      }

    // Check if should break
    bool no_difference = true;
    for (int i = 0; i < k; i++) {
      if (no_difference)
        for (int j = 0; j < 9; j++) {
          if (k_centroid[i][j] != (centroid_sum[i][j] / centroid_count[i])) {
            no_difference = false;
            break;
          }
        }
    }
    if (no_difference) {
      should_break = true;
    }

    for (int i = 0; i < k; i++)
      for (int j = 0; j < 9; j++) {
        k_centroid[i][j] = centroid_sum[i][j] / centroid_count[i];
      }
  }
  Image texture[4];
  for (int i = 0; i < k; i++) {
    texture[i] = Image(image_size, 1);
  }
  for (int i = 0; i < image_size; i++)
    for (int j = 0; j < image_size; j++) {
      texture[image_E.at(i, j, 0)].set(i, j, 255);
    }

  for (int t = 0; t < k; t++) {
    Close<9>(texture[t]);
    Open<9>(texture[t]);

    // Remove connected component that its area is too small
    Image texture_copy(texture[t]);
    Image labeled(image_size, 1);
    bool  empty                     = false;
    int   structuring_element[3][3] = { { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } };

    while (!empty) {
      empty = true;
      int first_i, first_j;
      for (int i = 0; i < image_size; i++)
        if (empty)
          for (int j = 0; j < image_size; j++) {
            if (texture_copy.at(i, j, 0) > 0) {
              empty   = false;
              first_i = i;
              first_j = j;
              break;
            }
          }
      if (!empty) {
        Image labeled_next(image_size, 1);
        labeled_next.set(first_i, first_j, 1.0);
        do {
          labeled = labeled_next;
          Dilation<3>(labeled, structuring_element, labeled_next);
          labeled_next.Intersection(texture_copy);
        } while (labeled_next != labeled);

        // Count area
        int area = 0;
        for (int i = 0; i < image_size; i++)
          for (int j = 0; j < image_size; j++) {
            if (labeled_next.at(i, j, 0) > 0) {
              area++;
              texture_copy.set(i, j, 0);
            }
          }

        if (area <= 1000) {
          for (int i = 0; i < image_size; i++)
            for (int j = 0; j < image_size; j++) {
              if (labeled_next.at(i, j, 0) > 0)
                texture[t].set(i, j, 0);
            }
        }
      }
    }
  }

  for (int i = 0; i < image_size; i++)
    for (int j = 0; j < image_size; j++) {
      image_E.set(i, j, 0, gray_level[image_E.at(i, j, 0)]);
    }

  // Save the result of image_E
  snprintf(path, sizeof(path), "%s/image_E.raw", result_dir);
  image_E.WriteRaw(path);

  Image image_E2(image_size, 1);
  for (int i = 0; i < image_size; i++)
    for (int j = 0; j < image_size; j++)
      for (int t = 0; t < k; t++) {
        if (texture[t].at(i, j, 0) > 0) {
          image_E2.set(i, j, 0, gray_level[t]);
          break;
        }
      }

  // Save the result of image_E2
  snprintf(path, sizeof(path), "%s/image_E2.raw", result_dir);
  image_E2.WriteRaw(path);

  // Fill the black pixels
  Image image_E3(image_E2);
  for (int i = 0; i < image_size; i++)
    for (int j = 0; j < image_size; j++) {
      if (image_E3.at(i, j, 0) == 0) {
        for (int r = 1; r < (image_size / 2); r++) {
          bool asigned = false;
          for (int x = -r; !asigned && x <= r; x++)
            for (int y = -r; !asigned && y <= r; y++) {
              for (int t = 0; t < k; t++)
                if (texture[t].at(i + x, j + y, 0) > 0) {
                  image_E3.set(i, j, 0, gray_level[t]);
                  asigned = true;
                  break;
                }
            }
          if (asigned) break;
        }
      }
    }

  // Save the result of image_E3
  snprintf(path, sizeof(path), "%s/image_E3.raw", result_dir);
  image_E3.WriteRaw(path);

  // (c) Generate another texture image by exchanging the types of different texture
  Image texture_masks[4];
  for (int t = 0; t < k; t++) {
    texture_masks[t] = Image(image_size, 1);
  }
  for (int i = 0; i < image_size; i++)
    for (int j = 0; j < image_size; j++) {
      for (int t = 0; t < k; t++) {
        if (image_E3.at(i, j, 0) == gray_level[t]) {
          texture_masks[t].set(i, j, 0, 255);
          break;
        }
      }
    }

  Image synthesized_texture[4];
  for (int t = 0; t < k; t++) {
    TextureSynthesis(sample2, texture_masks[t], image_size, synthesized_texture[t]);
  }

  Image image_G(image_size, 1);
  for (int t = 0; t < k; t++) {
    synthesized_texture[t].Intersection(texture_masks[(t + 1) % k]);
    image_G.Union(synthesized_texture[t]);
  }

  // Save the result of image_G
  snprintf(path, sizeof(path), "%s/image_G.raw", result_dir);
  image_G.WriteRaw(path);

  return 0;
}
