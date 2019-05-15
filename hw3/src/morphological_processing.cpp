#include "morphological_processing.h"

void InnerBoundaryExtraction(const Image& img, Image& dst) {
  int   image_size                = img.get_size();
  int   structuring_element[3][3] = { { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } };
  Image eroded(image_size, 1);

  Erosion<3>(img, structuring_element, eroded);
  for (int i = 0; i < image_size; i++)
    for (int j = 0; j < image_size; j++) {
      eroded.set(i, j, (img.at(i, j, 0) - eroded.at(i, j, 0)));
    }
  dst = eroded;
}

void InnerBoundaryExtraction(Image& img) {
  InnerBoundaryExtraction(img, img);
}

void OuterBoundaryExtraction(const Image& img, Image& dst) {
  int   image_size                = img.get_size();
  int   structuring_element[3][3] = { { 1, 1, 1 }, { 1, 1, 1 }, { 1, 1, 1 } };
  Image dilated(image_size, 1);

  Dilation<3>(img, structuring_element, dilated);
  for (int i = 0; i < image_size; i++)
    for (int j = 0; j < image_size; j++) {
      dilated.set(i, j, (dilated.at(i, j, 0) - img.at(i, j, 0)));
    }
  dst = dilated;
}

void OuterBoundaryExtraction(Image& img) {
  OuterBoundaryExtraction(img, img);
}

unordered_set<int> shrink[12] = {
  unordered_set<int>({            }),
  unordered_set<int>({ 0b01000000, 0b00010000, 0b00000100, 0b00000001}),
  unordered_set<int>({ 0b10000000, 0b00100000, 0b00001000, 0b00000010}),
  unordered_set<int>({ 0b11000000, 0b01100000, 0b00110000, 0b00011000,
                       0b00001100, 0b00000110, 0b00000011, 0b10000001 }),
  unordered_set<int>({ 0b11000001, 0b01110000, 0b00011100, 0b00000111}),
  unordered_set<int>({ 0b10110000, 0b10100001, 0b01101000, 0b11000010,
                       0b11100000, 0b00111000, 0b00001110, 0b10000011 }),
  unordered_set<int>({ 0b10110001, 0b01101100, 0b11110000, 0b11100001,
                       0b01111000, 0b00111100, 0b00011110, 0b00001111,
                       0b10000111, 0b11000011 }),
  unordered_set<int>({ 0b11110001, 0b01111100, 0b00011111, 0b11000111}),
  unordered_set<int>({ 0b11100011, 0b11111000, 0b00111110, 0b10001111}),
  unordered_set<int>({ 0b11110011, 0b11100111, 0b11111100, 0b11111001,
                       0b01111110, 0b00111111, 0b10011111, 0b11001111 }),
  unordered_set<int>({ 0b11110111, 0b11111101, 0b01111111, 0b11011111}),
  unordered_set<int>({            })
};

unordered_set<int> thin[12] = {
  unordered_set<int>({            }),
  unordered_set<int>({            }),
  unordered_set<int>({            }),
  unordered_set<int>({            }),
  unordered_set<int>({ 0b11000001, 0b01110000, 0b00011100, 0b00000111,
                       0b10100000, 0b00101000, 0b00001010, 0b10000010 }),
  unordered_set<int>({ 0b10110000, 0b10100001, 0b01101000, 0b11000010,
                       0b11100000, 0b00111000, 0b00001110, 0b10000011 }),
  unordered_set<int>({ 0b10110001, 0b01101100, 0b11110000, 0b11100001,
                       0b01111000, 0b00111100, 0b00011110, 0b00001111,
                       0b10000111, 0b11000011 }),
  unordered_set<int>({ 0b11110001, 0b01111100, 0b00011111, 0b11000111}),
  unordered_set<int>({ 0b11100011, 0b11111000, 0b00111110, 0b10001111}),
  unordered_set<int>({ 0b11110011, 0b11100111, 0b11111100, 0b11111001,
                       0b01111110, 0b00111111, 0b10011111, 0b11001111 }),
  unordered_set<int>({ 0b11110111, 0b11111101, 0b01111111, 0b11011111}),
  unordered_set<int>({            })
};

unordered_set<int> skeletonize[12] = {
  unordered_set<int>({            }),
  unordered_set<int>({            }),
  unordered_set<int>({            }),
  unordered_set<int>({            }),
  unordered_set<int>({ 0b11000001, 0b01110000, 0b00011100, 0b00000111,
                       0b10100000, 0b00101000, 0b00001010, 0b10000010 }),
  unordered_set<int>({            }),
  unordered_set<int>({  0b11110000, 0b11100001, 0b01111000, 0b00111100,
                        0b00011110, 0b00001111, 0b10000111, 0b11000011 }),
  unordered_set<int>({ 0b11110001, 0b01111100, 0b00011111, 0b11000111}),
  unordered_set<int>({ 0b11100011, 0b11111000, 0b00111110, 0b10001111}),
  unordered_set<int>({ 0b11110011, 0b11100111, 0b11111100, 0b11111001,
                       0b01111110, 0b00111111, 0b10011111, 0b11001111 }),
  unordered_set<int>({ 0b11110111, 0b11111101, 0b01111111, 0b11011111}),
  unordered_set<int>({ 0b11111011, 0b11111110, 0b10111111, 0b11101111})
};

int shrink_thin_unconditional[49][8] = {
  { 0, 1, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 1, 0 }, { 1, 0, 0, 0, 0, 0, 0, 0 },
  { 1, 1, 0, 0, 0, 0, 0, 0 }, { 0, 1, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 1, 0, 0, 0, 0 }, { 0, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 1, 0, 0 }, { 0, 0, 0, 0, 0, 1, 1, 0 },
  { 0, 0, 0, 0, 0, 0, 1, 1 }, { 1, 0, 0, 0, 0, 0, 0, 1 },
  { 0, 1, 1, 0, 1, 0, 0, 0 }, { 1, 0, 1, 1, 0, 0, 0, 0 },
  { 1, 0, 1, 0, 0, 0, 0, 1 }, { 1, 1, 0, 0, 0, 0, 1, 0 },
  { 1, 1, 2, 0, 0, 1, 0, 0 }, { 2, 1, 1, 0, 0, 1, 0, 0 },
  { 0, 0, 1, 1, 2, 0, 0, 1 }, { 0, 0, 2, 1, 1, 0, 0, 1 },
  { 0, 1, 0, 0, 1, 1, 2, 0 }, { 0, 1, 0, 0, 2, 1, 1, 0 },
  { 1, 0, 0, 1, 0, 0, 2, 1 }, { 2, 0, 0, 1, 0, 0, 1, 1 },
  { 2, 2, 1, 1, 1, 2, 2, 2 },
  { 1, 0, 1, 2, 1, 2, 0, 0 }, { 1, 2, 1, 0, 1, 0, 0, 2 },
  { 1, 2, 0, 0, 1, 0, 1, 2 }, { 1, 0, 0, 2, 1, 2, 1, 0 },
  { 0, 2, 1, 2, 1, 0, 1, 0 }, { 0, 0, 1, 0, 1, 2, 1, 2 },
  { 1, 0, 1, 0, 0, 2, 1, 2 }, { 1, 2, 1, 2, 0, 0, 1, 0 },
  { 2, 1, 2, 1, 2, 1, 2, 2 }, { 2, 1, 2, 1, 2, 2, 1, 2 }, { 2, 1, 2, 1, 2, 2, 2, 1 },
  { 1, 2, 2, 1, 2, 1, 2, 2 }, { 2, 1, 2, 1, 2, 1, 2, 2 }, { 2, 2, 2, 1, 2, 1, 2, 1 },
  { 2, 1, 2, 2, 2, 1, 2, 1 }, { 2, 2, 1, 2, 2, 1, 2, 1 }, { 2, 2, 2, 1, 2, 1, 2, 1 },
  { 2, 1, 2, 1, 2, 2, 2, 1 }, { 2, 1, 2, 2, 1, 2, 2, 1 }, { 2, 1, 2, 2, 2, 1, 2, 1 },
  { 1, 0, 1, 2, 0, 1, 0, 2 }, { 0, 2, 1, 0, 1, 2, 0, 1 },
  { 0, 1, 0, 2, 1, 0, 1, 2 }, { 1, 2, 0, 1, 0, 2, 1, 0 } };
int skeletonize_unconditional[34][8] = {
  { 0, 0, 0, 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 0, 1, 0, 0 },
  { 0, 1, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 1, 0 }, { 1, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 0, 0, 0 }, { 0, 0, 1, 0, 0, 0, 0, 0 },
  { 1, 0, 1, 0, 0, 0, 0, 0 }, { 0, 0, 1, 0, 1, 0, 0, 0 },
  { 1, 0, 0, 0, 0, 0, 1, 0 }, { 0, 0, 0, 0, 1, 0, 1, 0 },
  { 2, 2, 1, 1, 1, 2, 2, 2 }, { 1, 2, 2, 2, 2, 2, 1, 1 },
  { 1, 2, 1, 2, 1, 2, 2, 2 }, { 2, 2, 1, 2, 1, 2, 1, 2 },
  { 1, 2, 2, 2, 1, 2, 1, 2 }, { 1, 2, 1, 2, 2, 2, 1, 2 },
  { 2, 1, 2, 1, 2, 1, 2, 2 }, { 2, 1, 2, 1, 2, 2, 1, 2 }, { 2, 1, 2, 1, 2, 2, 2, 1 },
  { 1, 2, 2, 1, 2, 1, 2, 2 }, { 2, 1, 2, 1, 2, 1, 2, 2 }, { 2, 2, 2, 1, 2, 1, 2, 1 },
  { 2, 1, 2, 2, 2, 1, 2, 1 }, { 2, 2, 1, 2, 2, 1, 2, 1 }, { 2, 2, 2, 1, 2, 1, 2, 1 },
  { 2, 1, 2, 1, 2, 2, 2, 1 }, { 2, 1, 2, 2, 1, 2, 2, 1 }, { 2, 1, 2, 2, 2, 1, 2, 1 },
  { 1, 0, 1, 2, 0, 1, 0, 2 }, { 0, 2, 1, 0, 1, 2, 0, 1 },
  { 0, 1, 0, 2, 1, 0, 1, 2 }, { 1, 2, 0, 1, 0, 2, 1, 0 } };

void Thinning(const Image& img, Image& dst) {
  int   image_size = img.get_size();
  Image source(image_size, 1), processed(img);

  processed.Clamp();

  while (source != processed) {
    source = processed;

    // Thinning Stage 1 - Conditional Mark Patterns
    Image M(image_size, 1);

    for (int i = 0; i < image_size; i++) {
      for (int j = 0; j < image_size; j++) {
        if (source.at(i, j, 0) > 0) {
          int m              = 0;
          int bond           = source.Bond(i, j);
          int pixel_neighbor = source.NeighborPixelStack(i, j);
          for (auto it = thin[bond].begin(); it != thin[bond].end(); it++) {
            if (pixel_neighbor == (*it)) {
              m = 255;
              break;
            }
          }
          M.set(i, j, m);
        }
        else {
          M.set(i, j, 0);
        }
      }
    }

    // Thinning Stage 2 - Unconditional Mark Patterns
    Image P(image_size, 1);
    for (int i = 0; i < image_size; i++) {
      for (int j = 0; j < image_size; j++) {
        if (M.at(i, j, 0)) {
          int  mask_neighbor = M.NeighborPixelStack(i, j);
          bool hit           = false;
          for (int k = 0; k < 49; k++) {
            bool pattern_matches = true;
            for (int l = 0; l < 8; l++) {
              if ((shrink_thin_unconditional[k][l] == 0) || (shrink_thin_unconditional[k][l] == 1)) {
                if (((mask_neighbor >> (7 - l)) & 0b00000001) != shrink_thin_unconditional[k][l]) {
                  pattern_matches = false; break;
                }
              }
            }
            if (pattern_matches) {
              hit = true;
              break;
            }
          }
          P.set(i, j, hit ? 255 : 0);
        }
        else {
          P.set(i, j, 0);
        }
      }
    }
    M.Complement();
    M.Union(P);
    processed.Intersection(M);
    processed.Clamp();
  }

  dst = processed;
}

void Thinning(Image& img) {
  Thinning(img, img);
}

void Skeletonizing(const Image& img, Image& dst) {
  int   image_size = img.get_size();
  Image source(image_size, 1), processed(img);

  processed.Clamp();

  while (source != processed) {
    source = processed;

    // Skeletonizing Stage 1 - Conditional Mark Patterns
    Image M(image_size, 1);

    for (int i = 0; i < image_size; i++)
      for (int j = 0; j < image_size; j++) {
        if (source.at(i, j, 0)) {
          int m              = 0;
          int bond           = source.Bond(i, j);
          int pixel_neighbor = source.NeighborPixelStack(i, j);
          for (auto it = skeletonize[bond].begin(); it != skeletonize[bond].end(); it++) {
            if (pixel_neighbor == (*it)) {
              m = 255; break;
            }
          }
          M.set(i, j, m);
        }
        else {
          M.set(i, j, 0);
        }
      }

    // Skeletonizing Stage 2 - Unconditional Mark Patterns
    Image P(image_size, 1);
    for (int i = 0; i < image_size; i++)
      for (int j = 0; j < image_size; j++) {
        if (M.at(i, j, 0)) {
          int  mask_neighbor = M.NeighborPixelStack(i, j);
          bool hit           = false;
          for (int k = 0; k < 34; k++) {
            bool pattern_matches = true;
            for (int l = 0; l < 8; l++) {
              if ((skeletonize_unconditional[k][l] == 0) || (skeletonize_unconditional[k][l] == 1)) {
                if (((mask_neighbor >> (7 - l)) & 0b00000001) != skeletonize_unconditional[k][l]) {
                  pattern_matches = false;
                  break;
                }
              }
            }
            if (pattern_matches) {
              hit = true;
              break;
            }
          }
          P.set(i, j, hit ? 255 : 0);
        }
        else {
          P.set(i, j, 0);
        }
      }
    M.Complement();
    M.Union(P);
    processed.Intersection(M);
    processed.Clamp();
  }

  dst = processed;
}

void Skeletonizing(Image& img) {
  Skeletonizing(img, img);
}
