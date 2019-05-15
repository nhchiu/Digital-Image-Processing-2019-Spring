#ifndef _MORPHOLOGICAL_PROCESSING_H_
#define _MORPHOLOGICAL_PROCESSING_H_

#include "image.h"
#include <unordered_set>

using namespace std;

template<int w>
void Dilation(const Image& img, int structuring_element[w][w], Image& dst) {
  int   image_size = img.get_size();
  Image dilated(image_size, 1);
  int   offset = -(int)(w / 2);

  for (int i = 0; i < image_size; i++)
    for (int j = 0; j < image_size; j++) {
      dilated.set(i, j, 0.0);
    }
  for (int m = 0; m < w; m++)
    for (int n = 0; n < w; n++) {
      if (structuring_element[m][n]) {
        for (int i = 0; i < image_size; i++)
          for (int j = 0; j < image_size; j++) {
            dilated.set(i + m + offset, j + n + offset,
                        max(dilated.at(i + m + offset, j + n + offset, 0), img.at(i, j, 0)));
          }
      }
    }
  dst = dilated;
}

template<int w>
void Erosion(const Image& img, int structuring_element[w][w], Image& dst) {
  int   image_size = img.get_size();
  Image eroded(image_size, 1);
  int   offset = -(int)(w / 2);

  for (int i = 0; i < image_size; i++)
    for (int j = 0; j < image_size; j++) {
      eroded.set(i, j, 1.0);
    }
  for (int m = 0; m < w; m++)
    for (int n = 0; n < w; n++) {
      if (structuring_element[m][n]) {
        for (int i = 0; i < image_size; i++)
          for (int j = 0; j < image_size; j++) {
            eroded.set(i + m + offset, j + n + offset,
                       min(eroded.at(i + m + offset, j + n + offset, 0), img.at(i, j, 0)));
          }
      }
    }
  dst = eroded;
}

void InnerBoundaryExtraction(const Image& img, Image& dst);
void InnerBoundaryExtraction(Image& img);
void OuterBoundaryExtraction(const Image& img, Image& dst);
void OuterBoundaryExtraction(Image& img);
void Thinning(const Image& img, Image& dst);
void Thinning(Image& img);
void Skeletonizing(const Image& img, Image& dst);
void Skeletonizing(Image& img);

template<int s>
void Open(const Image& img, Image& dst) {
  int image_size                = img.get_size();
  int structuring_element[s][s] = { { 0 } };

  for (int i = 0; i < s; i++)
    for (int j = 0; j < s; j++) {
      if (((i - (s / 2 + 1)) * (i - (s / 2 + 1)) +
           (j - (s / 2 + 1)) * (j - (s / 2 + 1))) <= (s / 2 + 2) * (s / 2 + 2)) {
        structuring_element[i][j] = 1;
      }
    }
  Image opened(image_size, 1);

  Erosion<s>(img, structuring_element, opened);
  Dilation<s>(opened, structuring_element, opened);
  dst = opened;
}

template<int s>
void Open(Image& img) {
  Open<s>(img, img);
}

template<int s>
void Close(const Image& img, Image& dst) {
  int image_size = img.get_size();
  int structuring_element[s][s];

  for (int i = 0; i < s; i++)
    for (int j = 0; j < s; j++) {
      structuring_element[i][j] = 1;
    }
  Image closed(image_size, 1);

  Dilation<s>(img, structuring_element, closed);
  Erosion<s>(closed, structuring_element, closed);
  dst = closed;
}

template<int s>
void Close(Image& img) {
  Close<s>(img, img);
}

#endif // _MORPHOLOGICAL_PROCESSING_H_
