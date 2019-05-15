/* MORPHOLOGICAL PROCESSING */

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "image.h"
#include "morphological_processing.h"

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

  // read image sample1 into image data matrix
  int   image_size = 256;
  Image sample1(image_size, 1);
  snprintf(path, sizeof(path), "%s/sample1.raw", source_image_dir);
  sample1.ReadRaw(path);

  // (a) Perform boundary extraction on I_1
  Image image_B = Image(image_size, 1);

  // Calculate gradient magnitude
  InnerBoundaryExtraction(sample1, image_B);

  // Save results
  snprintf(path, sizeof(path), "%s/image_B.raw", result_dir);
  image_B.WriteRaw(path);

  // (b) Perform connected component labeling on I_1
  Image image_C(image_size, 3);

  Image  sample1_copy(sample1);
  Image  labeled(image_size, 1);
  bool   empty                     = false;
  int    structuring_element[3][3] = { { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } };
  int    image_count               = 0;
  double color[10][3]              = {
    { 1.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0 },
    { 0.0, 0.0, 1.0 },
    { 0.5, 0.7, 0.0 },
    { 0.0, 0.5, 0.7 },
    { 0.7, 0.0, 0.5 },
    { 0.4, 0.4, 0.6 },
    { 1.0, 0.8, 0.1 },
    { 0.1, 1.0, 0.8 },
    { 0.8, 0.1, 1.0 } };

  while (!empty) {
    empty = true;
    int first_i, first_j;
    for (int i = 0; i < image_size; i++) {
      if (empty)
        for (int j = 0; j < image_size; j++) {
          if (sample1_copy.at(i, j, 0) > 0) {
            empty   = false;
            first_i = i;
            first_j = j;
            break;
          }
        }
    }
    if (!empty) {
      Image labeled_next(image_size, 1);
      labeled_next.set(first_i, first_j, 1.0);
      do
      {
        labeled = labeled_next;
        Dilation<3>(labeled, structuring_element, labeled_next);
        labeled_next.Intersection(sample1_copy);
      } while (labeled_next != labeled);

      for (int i = 0; i < image_size; i++) {
        for (int j = 0; j < image_size; j++) {
          if (labeled_next.at(i, j, 0) > 0) {
            sample1_copy.set(i, j, 0.0);
            image_C.set(i, j, color[image_count % 10]);
          }
        }
      }
      image_count++;
    }
  }

  // Save results
  snprintf(path, sizeof(path), "%s/image_C.raw", result_dir);
  image_C.WriteRaw(path);

  // (c) Perform thinning and skeletonizing on I_1 and output the results as image D1 and D2
  Image image_D1, image_D2;
  fprintf(stderr, "Thinning\n");
  Thinning(sample1, image_D1);
  fprintf(stderr, "Skeletonizing\n");
  Skeletonizing(sample1, image_D2);

  // Save results
  snprintf(path, sizeof(path), "%s/image_D1.raw", result_dir);
  image_D1.WriteRaw(path);
  snprintf(path, sizeof(path), "%s/image_D2.raw", result_dir);
  image_D2.WriteRaw(path);

  return 0;
}
